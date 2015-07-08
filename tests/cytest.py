'''
cytest - A module to compare Curry output.
'''

from __future__ import print_function
import cStringIO
import os
import re
import shutil
import subprocess
import sys
import tokenize
import unittest

# OK to change the next value.
VERBOSE = True

if VERBOSE:
  def log(*args):
      sys.stdout.write('[INFO]: ')
      print(*args)
else:
  def log(*args): pass

def top_help(exitcode, stream):
  stream.write(
      '''
      USAGE: %s [clean|compare|help|validate] filenames...

          clean
              Clean up any changes to the .curry source files caused by 'mkgold'.
          compare
              Compare the results produced by Sprite with the results produced
              the oracle for one or more .curry files.
          help
              Display this help message.
          mkgold
              Generate golden results by running the oracle and appending its result
              to the .curry file.
          validate
              Validate the comparisons of a previous call to 'compare'.  The
              results are stored in files ending with .results.
      ''' % sys.argv[0]
    )
  os._exit(exitcode)

# Note: warnings.warn does not interoperate well with unittest.  Moreover,
# shell directs cannot redirect stdout and stderr to a file while printing a
# copy of stderr to tty.  Prepend $$ to identify error messages.
def warn(msg):
  sys.stderr.write('\n')
  sys.stderr.write('$$ ')
  sys.stderr.write(msg.replace('\n', '\n$$ '))
  sys.stderr.write('\n')

def syscall(progname, args=''):
  '''
  Calls a system function and captures the contents written to stdout, split on
  newlines.  Returns the captured output and the process return code (mapped to
  {0,1}).
  '''
  cmd = '%s %s' % (progname, args)
  log('Running command:', cmd)
  process = subprocess.Popen(
      cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True
    )
  (out,err) = process.communicate()

  # Special case for PAKCS.  If there is no value, PAKCSC exits with status 2.
  if progname == 'pakcs' and process.returncode == 2:
    return (out, 0)

  if err:
    args = (
        sys.argv[0]
      , os.path.basename(progname)
      , err.strip().replace('\n', '\n        ')
      )
    warn('%s: %s generates error output:\n\n        %s' % args)
  return out, 0 if process.returncode == 0 else 1

def extractAnswers((string, rc), do_sort=True, kics_handling=False):
  lines = string.split('\n')
  if kics_handling:
    i = 0
    n = len(lines)
    while i<n and not lines[i].startswith('Evaluating expression: main'):
      i += 1
    lines = lines[i+1:]
    if rc != 0:
      # Discard warning (to stdout).
      lines = filter(
          lambda s: not s.startswith('Evaluation terminated with non-zero status')
        , lines
        )
  answer = [line for line in lines if line]
  return (sorted(answer) if do_sort else answer, rc)

match_answer = re.compile('-->\s+(.*)')
match_rc = re.compile('--\$\?->\s+(\d+)')
banner = 'CORRECT ANSWER BELOW GENERATED BY cytest.py'
match_banner = re.compile(banner)

def getOracleAnswers(filename):
  if not any(map(match_banner.search, open(filename, 'r').readlines())):
    if open(filename, 'r').readline().startswith('-- KICS2 --'):
      oracle,opt_q,opt_h = 'kics2','',True
    else:
      oracle,opt_q,opt_h = 'pakcs','-q',False

    oracle_answer, oracle_rc = extractAnswers(
        syscall(oracle, '%s :l %s :eval main :q' % (opt_q, filename))
      , kics_handling=opt_h
      )
    with open(filename, 'a') as cyfile:
      cyfile.write('\n------ %s using %s ------\n' % (banner, oracle))
      cyfile.write(''.join([ '--> %s\n' % line for line in oracle_answer ]))
      cyfile.write('--$?-> %s\n' % oracle_rc)
  else:
    oracle_answer = []
    oracle_rc = None
    for line in open(filename, 'r').readlines():
      match = match_answer.match(line)
      if match:
        oracle_answer.append(match.group(1))
        continue
      match = match_rc.match(line)
      if match:
        if oracle_rc is not None:
          warn('Multiple return codes!')
          os._exit(1)
        oracle_rc = int(match.group(1))
    # Assume success.
    if oracle_rc is None:
      oracle_rc = 0
  return oracle_answer, oracle_rc

def getAnswers(filename):
  log('Processing file:', filename)
  oracle_answer, oracle_rc = getOracleAnswers(filename)
  
  # Get the Sprite answer.
  head,tail = os.path.splitext(filename)
  if tail != '' and tail != '.curry':
    warn('%s: unexpected file extension: %s' % (sys.argv[0], tail))

  exe = os.path.abspath(head + '.exe')
  # syscall('scc', '-o %s %s' % (exe, filename))
  sprite_answer, sprite_rc = extractAnswers(syscall(exe))
  log('Oracle answer: ', oracle_answer)
  log('Sprite answer:', sprite_answer)
  log('Oracle return code: ', oracle_rc)
  log('Sprite return code:', sprite_rc)

  return oracle_answer, sprite_answer, oracle_rc, sprite_rc

class Tokenizer(object):
  '''
  Tokenizes and compares result strings.

  Implements a loose comparison by:
    - Converting KiCS2-style free variable strings to PACKS-style ones.
    - Ignoring differences in whether the whole expression has parens.
  '''

  def __init__(self):
    self.symbols = {}
    self.pat = re.compile('_x\d+')
    self.nextid = 0
    self.out = []

  def _show(self, i):
    if i < 26:
      return '_' + chr(97 + i)
    else:
      return '_a%d' % i

  def finalize(self):
    '''Ignore differences in outermost parenthesization.'''
    if self.out[0] == '(' and self.out[-1] == ')':
      self.out = self.out[1:-1]

  def __call__(self, ty, text, rc_beg, rc_end, lineno):
    if text == '':
      return

    if self.pat.match(text):
      if text not in self.symbols:
        self.symbols[text] = self.nextid
        self.nextid += 1
      self.out.append(self._show(self.symbols[text]))
    else:
      self.out.append(text)

  def __eq__(self, rhs):
    return self.out == rhs.out
    

def processAnswer(text):
  tok = Tokenizer()
  tokenize.tokenize(cStringIO.StringIO(text).readline, tok)
  tok.finalize()
  return tok

def equalAnswers(tc, oracle, sprite, oracle_rc, sprite_rc):
  tc.assertEqual(
      len(oracle), len(sprite)
    , msg='len(oracle) != len(sprite)'
    )
  for p,s in zip(oracle, sprite):
    p,s = map(processAnswer, (p,s))
    tc.assertTrue(p == s, msg='Oracle=%s  Sprite=%s' % (p, s))
  tc.assertEqual(
      oracle_rc, sprite_rc
     , msg='return codes do not agree: Oracle=%s  Sprite=%s' % (oracle_rc, sprite_rc)
    )

def runTestSuite(tc):
  suite = unittest.TestLoader().loadTestsFromTestCase(tc)
  return unittest.TextTestRunner(stream=sys.stdout, verbosity=2).run(suite)

def top_clean():
  for filename in sys.argv[2:]:
    filename_copy = filename + '.copy'
    with open(filename_copy, 'w') as cp:
      for line in open(filename, 'r').readlines():
        if match_banner.search(line):
          break;
        cp.write(line)
    shutil.move(filename_copy, filename)

def top_mkgold():
  for filename in sys.argv[2:]:
    getOracleAnswers(filename)

def top_compare():
  class CompareTC(unittest.TestCase): pass
  for filename in sys.argv[2:]:
    base = os.path.splitext(os.path.basename(filename))[0]
    def test(self): equalAnswers(self, *getAnswers(filename))
    setattr(CompareTC, 'test_' + base, test)
  rv = runTestSuite(CompareTC)
  # The compare step succeeds as long as the tests were run (regardless of
  # whether or not they passed).
  sys.exit(0 if rv.testsRun == len(sys.argv[2:]) else 1)

def top_validate():
  class ValidateTC(unittest.TestCase): pass
  for filename in sys.argv[2:]:
    base = os.path.splitext(os.path.basename(filename))[0]
    def test(self):
      text = open(filename, 'r').readlines()[-1].strip()
      self.assertEquals(text, 'OK')
    setattr(ValidateTC, 'test_' + base, test)
  rv = runTestSuite(ValidateTC)
  sys.exit(0 if rv.wasSuccessful() else 1)

if __name__ == '__main__':
  command = sys.argv[1]
  if command == 'clean':
    top_clean()
  elif command == 'compare':
    top_compare()
  elif command == 'mkgold':
    top_mkgold()
  elif command == 'help':
    top_help(0, sys.stdout)
  elif command == 'validate':
    top_validate()
  else:
    sys.stderr.write('Invalid command: ' + command)
    top_help(1, sys.stderr)

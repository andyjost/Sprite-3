'''
cytest - A module to compare Curry output.
'''

from __future__ import print_function
import os
import re
import shutil
import subprocess
import sys
import unittest

# OK to change the next value.
VERBOSE = False

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
              by PAKCS for one or more .curry files.
          help
              Display this help message.
          mkgold
              Generate golden results by running PAKCS and appending its result
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
  Calls a system function and returns the contents written to stdout, split on
  newlines.  If the process writies anything to stderr, issues a warning.
  '''
  cmd = '%s %s' % (progname, args)
  log('Running command:', cmd)
  process = subprocess.Popen(
      cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True
    )
  (out,err) = process.communicate()

  # Special case for PAKCS.  If there is no value, PAKCSC exits with status 2.
  if progname == 'pakcs' and process.returncode == 2:
    return out

  if err:
    args = (
        sys.argv[0]
      , os.path.basename(progname)
      , err.strip().replace('\n', '\n        ')
      )
    warn('%s: %s generates error output:\n\n        %s' % args)
  if process.returncode != 0:
    warn(
        '%s: %s exited with status %s'
            % (sys.argv[0], os.path.basename(progname), process.returncode)
      )
    os._exit(1)
  return out

def extractAnswers(string, do_sort=True):
  answer = [line for line in string.split('\n') if line]
  return sorted(answer) if do_sort else answer

match_answer = re.compile('-->\s+(.*)')
banner = 'CORRECT ANSWER BELOW GENERATED BY cytest.py'
match_banner = re.compile(banner)

def getPakcsAnswers(filename):
  if not any(map(match_banner.search, open(filename, 'r').readlines())):
    pakcs_answer = extractAnswers(
        syscall('pakcs', '-q :l %s :eval main :q' % filename)
      )
    with open(filename, 'a') as cyfile:
      cyfile.write('\n------ ')
      cyfile.write(banner)
      cyfile.write(' ------\n')
      cyfile.write('\n'.join([ '--> ' + line for line in pakcs_answer ]))
      cyfile.write('\n')
  else:
    pakcs_answer = [
        match.group(1)
            for match in map(match_answer.match, open(filename, 'r').readlines())
            if match
      ]
  return pakcs_answer

def getAnswers(filename):
  log('Processing file:', filename)
  pakcs_answer = getPakcsAnswers(filename)
  
  # Get the Sprite answer.
  head,tail = os.path.splitext(filename)
  if tail != '' and tail != '.curry':
    warn('%s: unexpected file extension: %s' % (sys.argv[0], tail))

  exe = os.path.abspath(head + '.exe')
  # syscall('scc', '-o %s %s' % (exe, filename))
  sprite_answer = extractAnswers(syscall(exe))
  log('PAKCS answer: ', pakcs_answer)
  log('Sprite answer:', sprite_answer)

  return pakcs_answer, sprite_answer

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
    getPakcsAnswers(filename)

def top_compare():
  class CompareTC(unittest.TestCase): pass
  for filename in sys.argv[2:]:
    base = os.path.splitext(os.path.basename(filename))[0]
    def test(self): self.assertEquals(*getAnswers(filename))
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
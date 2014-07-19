; ModuleID = 'plaintest.bc'

%vtable = type { i8* ()*, i64, void (%node*)*, void (%node*)* }
%node = type { %vtable*, i64, i8*, i8* }
%FILE = type opaque

@.str = private unnamed_addr constant [3 x i8] c" (\00", align 1
@.str1 = private unnamed_addr constant [2 x i8] c" \00", align 1
@.str2 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@.str3 = private unnamed_addr constant [2 x i8] c")\00", align 1
@.vtable.for.MyCons = internal global %vtable { i8* ()* @.name, i64 2, void (%node*)* @.N.2, void (%node*)* @.nullstep }
@.vtable.for.MyNil = internal global %vtable { i8* ()* @.name4, i64 0, void (%node*)* @.N.0, void (%node*)* @.nullstep }
@.vtable.for.Zero = internal global %vtable { i8* ()* @.name5, i64 0, void (%node*)* @.N.0, void (%node*)* @.nullstep }
@.vtable.for.Succ = internal global %vtable { i8* ()* @.name6, i64 1, void (%node*)* @.N.1, void (%node*)* @.nullstep }
@.vtable.for.half = internal global %vtable { i8* ()* @.name7, i64 1, void (%node*)* @.N.half, void (%node*)* @.H.half }
@.str8 = private unnamed_addr constant [16 x i8] c"FAIL case hit.\0A\00", align 1
@.str9 = private unnamed_addr constant [15 x i8] c"FWD case hit.\0A\00", align 1
@.str10 = private unnamed_addr constant [18 x i8] c"CHOICE case hit.\0A\00", align 1
@.str11 = private unnamed_addr constant [16 x i8] c"OPER case hit.\0A\00", align 1
@.str12 = private unnamed_addr constant [16 x i8] c"FAIL case hit.\0A\00", align 1
@.str13 = private unnamed_addr constant [15 x i8] c"FWD case hit.\0A\00", align 1
@.str14 = private unnamed_addr constant [18 x i8] c"CHOICE case hit.\0A\00", align 1
@.str15 = private unnamed_addr constant [16 x i8] c"OPER case hit.\0A\00", align 1
@.jtable = internal global [6 x i8*] [i8* blockaddress(@.step.half, %35), i8* blockaddress(@.step.half, %37), i8* blockaddress(@.step.half, %39), i8* blockaddress(@.step.half, %41), i8* blockaddress(@.step.half, %43), i8* blockaddress(@.step.half, %46)]
@.jtable16 = internal global [6 x i8*] [i8* blockaddress(@.step.half, %10), i8* blockaddress(@.step.half, %12), i8* blockaddress(@.step.half, %14), i8* blockaddress(@.step.half, %16), i8* blockaddress(@.step.half, %18), i8* blockaddress(@.step.half, %21)]
@.str17 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1
@.str18 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1

declare i32 @snprintf(i8*, i64, i8*, ...)

declare %FILE* @fmemopen(i8*, i64, i8*)

declare i32 @fprintf(%FILE*, i8*, ...)

declare i32 @printf(i8*, ...)

declare i32 @fflush(%FILE*)

declare i32 @fclose(%FILE*)

declare i8* @malloc(i64)

declare void @free(i8*)

define linkonce void @printexpr(%node* %root_p, i1 %is_outer) {
.entry:
  %0 = getelementptr %node* %root_p, i32 0, i32 0
  %1 = load %vtable** %0
  %2 = getelementptr %vtable* %1, i32 0, i32 1
  %3 = load i64* %2
  %4 = icmp eq i1 %is_outer, false
  %5 = icmp ugt i64 %3, 0
  br i1 %4, label %6, label %7

; <label>:6                                       ; preds = %.entry
  br i1 %5, label %19, label %17

; <label>:7                                       ; preds = %21, %.entry
  %8 = getelementptr %node* %root_p, i32 0, i32 0
  %9 = load %vtable** %8
  %10 = getelementptr %vtable* %9, i32 0, i32 0
  %11 = load i8* ()** %10
  %12 = call i8* %11()
  %13 = call i32 (i8*, ...)* @printf(i8* getelementptr inbounds ([3 x i8]* @.str2, i32 0, i32 0), i8* %12)
  %14 = alloca i64
  store i64 0, i64* %14
  %15 = getelementptr %node* %root_p, i32 0, i32 2
  %16 = load i8** %15
  br label %26

; <label>:17                                      ; preds = %6
  %18 = call i32 (i8*, ...)* @printf(i8* getelementptr inbounds ([2 x i8]* @.str1, i32 0, i32 0))
  br label %21, !sprite.implied !0

; <label>:19                                      ; preds = %6
  %20 = call i32 (i8*, ...)* @printf(i8* getelementptr inbounds ([3 x i8]* @.str, i32 0, i32 0))
  br label %21, !sprite.implied !0

; <label>:21                                      ; preds = %19, %17
  br label %7, !sprite.implied !0

; <label>:22                                      ; preds = %26
  %23 = bitcast i8* %16 to %node*
  call void @printexpr(%node* %23, i1 false)
  %24 = load i64* %14
  %25 = add i64 %24, 1
  store i64 %25, i64* %14
  br label %26, !sprite.implied !1

; <label>:26                                      ; preds = %22, %7
  %27 = load i64* %14
  %28 = icmp ult i64 %27, %3
  br i1 %28, label %22, label %29

; <label>:29                                      ; preds = %26
  br i1 %4, label %30, label %31

; <label>:30                                      ; preds = %29
  br i1 %5, label %32, label %34

; <label>:31                                      ; preds = %34, %29
  ret void

; <label>:32                                      ; preds = %30
  %33 = call i32 (i8*, ...)* @printf(i8* getelementptr inbounds ([2 x i8]* @.str3, i32 0, i32 0))
  br label %34, !sprite.implied !0

; <label>:34                                      ; preds = %32, %30
  br label %31, !sprite.implied !0
}

define linkonce void @.nullstep(%node*) {
.entry:
  ret void
}

define linkonce void @.N.2(%node* %root_p) {
.entry:
  %0 = getelementptr %node* %root_p, i32 0, i32 3
  %1 = load i8** %0
  %2 = bitcast i8* %1 to %node*
  %3 = getelementptr %node* %2, i32 0, i32 0
  %4 = load %vtable** %3
  %5 = getelementptr %vtable* %4, i32 0, i32 2
  %6 = load void (%node*)** %5
  call void %6(%node* %2)
  %7 = getelementptr %node* %root_p, i32 0, i32 2
  %8 = load i8** %7
  %9 = bitcast i8* %8 to %node*
  %10 = getelementptr %node* %9, i32 0, i32 0
  %11 = load %vtable** %10
  %12 = getelementptr %vtable* %11, i32 0, i32 2
  %13 = load void (%node*)** %12
  tail call void %13(%node* %9)
  ret void
}

declare internal i8* @.name()

define linkonce void @.N.0(%node* %root_p) {
.entry:
  ret void
}

declare internal i8* @.name4()

declare internal i8* @.name5()

define linkonce void @.N.1(%node* %root_p) {
.entry:
  %0 = getelementptr %node* %root_p, i32 0, i32 2
  %1 = load i8** %0
  %2 = bitcast i8* %1 to %node*
  %3 = getelementptr %node* %2, i32 0, i32 0
  %4 = load %vtable** %3
  %5 = getelementptr %vtable* %4, i32 0, i32 2
  %6 = load void (%node*)** %5
  tail call void %6(%node* %2)
  ret void
}

declare internal i8* @.name6()

define linkonce void @.step.half(%node* %root_p) {
.entry:
  %0 = getelementptr %node* %root_p, i32 0, i32 2
  %1 = load i8** %0
  %2 = bitcast i8* %1 to %node*
  %3 = bitcast %node* %2 to i8*
  %4 = bitcast i8* %3 to %node*
  %5 = getelementptr %node* %4, i32 0, i32 1
  %6 = load i64* %5
  %7 = add i64 %6, 4
  %8 = getelementptr [6 x i8*]* @.jtable16, i32 0, i64 %7
  %9 = load i8** %8
  indirectbr i8* %9, [label %10, label %12, label %14, label %16, label %18, label %21]

; <label>:10                                      ; preds = %.entry
  %11 = call i32 (i8*, ...)* @printf(i8* getelementptr inbounds ([16 x i8]* @.str8, i32 0, i32 0))
  ret void, !case...FAIL !2

; <label>:12                                      ; preds = %.entry
  %13 = call i32 (i8*, ...)* @printf(i8* getelementptr inbounds ([15 x i8]* @.str9, i32 0, i32 0))
  ret void, !case...FWD !2

; <label>:14                                      ; preds = %.entry
  %15 = call i32 (i8*, ...)* @printf(i8* getelementptr inbounds ([18 x i8]* @.str10, i32 0, i32 0))
  ret void, !case...CHOICE !2

; <label>:16                                      ; preds = %.entry
  %17 = call i32 (i8*, ...)* @printf(i8* getelementptr inbounds ([16 x i8]* @.str11, i32 0, i32 0))
  ret void, !case...OPER !2

; <label>:18                                      ; preds = %.entry
  %19 = getelementptr %node* %root_p, i32 0, i32 0
  store %vtable* @.vtable.for.Zero, %vtable** %19
  %20 = getelementptr %node* %root_p, i32 0, i32 1
  store i64 0, i64* %20
  ret void

; <label>:21                                      ; preds = %.entry
  %22 = getelementptr %node* %root_p, i32 0, i32 2
  %23 = load i8** %22
  %24 = bitcast i8* %23 to %node*
  %25 = getelementptr %node* %24, i32 0, i32 2
  %26 = load i8** %25
  %27 = bitcast i8* %26 to %node*
  %28 = bitcast %node* %27 to i8*
  %29 = bitcast i8* %28 to %node*
  %30 = getelementptr %node* %29, i32 0, i32 1
  %31 = load i64* %30
  %32 = add i64 %31, 4
  %33 = getelementptr [6 x i8*]* @.jtable, i32 0, i64 %32
  %34 = load i8** %33
  indirectbr i8* %34, [label %35, label %37, label %39, label %41, label %43, label %46]

; <label>:35                                      ; preds = %21
  %36 = call i32 (i8*, ...)* @printf(i8* getelementptr inbounds ([16 x i8]* @.str12, i32 0, i32 0))
  ret void, !case...FAIL !2

; <label>:37                                      ; preds = %21
  %38 = call i32 (i8*, ...)* @printf(i8* getelementptr inbounds ([15 x i8]* @.str13, i32 0, i32 0))
  ret void, !case...FWD !2

; <label>:39                                      ; preds = %21
  %40 = call i32 (i8*, ...)* @printf(i8* getelementptr inbounds ([18 x i8]* @.str14, i32 0, i32 0))
  ret void, !case...CHOICE !2

; <label>:41                                      ; preds = %21
  %42 = call i32 (i8*, ...)* @printf(i8* getelementptr inbounds ([16 x i8]* @.str15, i32 0, i32 0))
  ret void, !case...OPER !2

; <label>:43                                      ; preds = %21
  %44 = getelementptr %node* %root_p, i32 0, i32 0
  store %vtable* @.vtable.for.Zero, %vtable** %44
  %45 = getelementptr %node* %root_p, i32 0, i32 1
  store i64 0, i64* %45
  ret void

; <label>:46                                      ; preds = %21
  %47 = call i8* @malloc(i64 32)
  %48 = bitcast i8* %47 to %node*
  %49 = getelementptr %node* %48, i32 0, i32 2
  %50 = load i8** %49
  %51 = bitcast i8* %50 to %node*
  %52 = getelementptr %node* %51, i32 0, i32 2
  %53 = load i8** %52
  %54 = bitcast i8* %53 to %node*
  %55 = getelementptr %node* %54, i32 0, i32 2
  %56 = load i8** %55
  %57 = bitcast i8* %56 to %node*
  %58 = bitcast %node* %57 to i8*
  %59 = getelementptr %node* %48, i32 0, i32 0
  store %vtable* @.vtable.for.half, %vtable** %59
  %60 = getelementptr %node* %48, i32 0, i32 1
  store i64 -1, i64* %60
  %61 = getelementptr %node* %48, i32 0, i32 2
  store i8* %58, i8** %61
  %62 = getelementptr %node* %root_p, i32 0, i32 0
  store %vtable* @.vtable.for.Succ, %vtable** %62
  %63 = getelementptr %node* %root_p, i32 0, i32 1
  store i64 1, i64* %63
  %64 = getelementptr %node* %root_p, i32 0, i32 2
  store i8* %47, i8** %64
  ret void
}

define linkonce void @.N.half(%node* %root_p) {
.entry:
  call void @.step.half(%node* %root_p)
  %0 = getelementptr %node* %root_p, i32 0, i32 0
  %1 = load %vtable** %0
  %2 = getelementptr %vtable* %1, i32 0, i32 2
  %3 = load void (%node*)** %2
  tail call void %3(%node* %root_p)
  ret void
}

define linkonce void @.H.half(%node* %root_p) {
.entry:
  call void @.step.half(%node* %root_p)
  %0 = getelementptr %node* %root_p, i32 0, i32 0
  %1 = load %vtable** %0
  %2 = getelementptr %vtable* %1, i32 0, i32 3
  %3 = load void (%node*)** %2
  tail call void %3(%node* %root_p)
  ret void
}

declare internal i8* @.name7()

define i32 @main() {
.entry:
  %0 = call i8* @malloc(i64 32)
  %1 = bitcast i8* %0 to %node*
  %2 = call i8* @malloc(i64 32)
  %3 = bitcast i8* %2 to %node*
  %4 = call i8* @malloc(i64 32)
  %5 = bitcast i8* %4 to %node*
  %6 = getelementptr %node* %5, i32 0, i32 0
  store %vtable* @.vtable.for.Zero, %vtable** %6
  %7 = getelementptr %node* %5, i32 0, i32 1
  store i64 0, i64* %7
  %8 = getelementptr %node* %3, i32 0, i32 0
  store %vtable* @.vtable.for.Succ, %vtable** %8
  %9 = getelementptr %node* %3, i32 0, i32 1
  store i64 1, i64* %9
  %10 = getelementptr %node* %3, i32 0, i32 2
  store i8* %4, i8** %10
  %11 = getelementptr %node* %1, i32 0, i32 0
  store %vtable* @.vtable.for.half, %vtable** %11
  %12 = getelementptr %node* %1, i32 0, i32 1
  store i64 -1, i64* %12
  %13 = getelementptr %node* %1, i32 0, i32 2
  store i8* %2, i8** %13
  call void @printexpr(%node* %1, i1 true)
  %14 = call i32 (i8*, ...)* @printf(i8* getelementptr inbounds ([2 x i8]* @.str17, i32 0, i32 0))
  %15 = getelementptr %node* %1, i32 0, i32 0
  %16 = load %vtable** %15
  %17 = getelementptr %vtable* %16, i32 0, i32 2
  %18 = load void (%node*)** %17
  call void %18(%node* %1)
  call void @printexpr(%node* %1, i1 true)
  %19 = call i32 (i8*, ...)* @printf(i8* getelementptr inbounds ([2 x i8]* @.str18, i32 0, i32 0))
  ret i32 0
}

!0 = metadata !{i32 0}
!1 = metadata !{i32 1}
!2 = metadata !{}

; RUN: opt < %s %loadEnzyme -enzyme -enzyme_preopt=false -mem2reg -instsimplify -simplifycfg -S | FileCheck %s

; __attribute__((noinline))
; double* cast(double *x) {
;     return x;
; }
; 
; __attribute__((noinline))
; void function(double y, double z, double *x) {
;     double m = y * z;
;     double* cs = cast(x);
;     //double* cs = cast(cast(x));
;     *cs = m;
; }
; 
; __attribute__((noinline))
; void addOne(double *x) {
;     *x += 1;
; }
; 
; __attribute__((noinline))
; void function0(double y, double z, double *x) {
;     function(y, z, x);
;     addOne(x);
; }
; 
; double test_derivative(double *x, double *xp, double y, double z) {
;   return __builtin_autodiff(function0, y, z, x, xp);
; }

; Function Attrs: noinline norecurse nounwind readnone uwtable
define dso_local double* @cast(double* readnone returned %x) local_unnamed_addr #0 {
entry:
  ret double* %x
}

; Function Attrs: noinline norecurse nounwind uwtable
define dso_local void @function(double %y, double %z, double* %x) local_unnamed_addr #1 {
entry:
  %mul = fmul fast double %z, %y
  %call = tail call double* @cast(double* %x)
  store double %mul, double* %call, align 8, !tbaa !2
  ret void
}

; Function Attrs: noinline norecurse nounwind uwtable
define dso_local void @addOne(double* nocapture %x) local_unnamed_addr #1 {
entry:
  %0 = load double, double* %x, align 8, !tbaa !2
  %add = fadd fast double %0, 1.000000e+00
  store double %add, double* %x, align 8, !tbaa !2
  ret void
}

; Function Attrs: noinline norecurse nounwind uwtable
define dso_local void @function0(double %y, double %z, double* %x) #1 {
entry:
  tail call void @function(double %y, double %z, double* %x)
  tail call void @addOne(double* %x)
  ret void
}

; Function Attrs: nounwind uwtable
define dso_local double @test_derivative(double* %x, double* %xp, double %y, double %z) local_unnamed_addr #2 {
entry:
  %0 = tail call double (void (double, double, double*)*, ...) @__enzyme_autodiff(void (double, double, double*)* nonnull @function0, double %y, double %z, double* %x, double* %xp)
  ret double %0
}

; Function Attrs: nounwind
declare double @__enzyme_autodiff(void (double, double, double*)*, ...) #3

attributes #0 = { noinline norecurse nounwind readnone uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="false" "no-infs-fp-math"="true" "no-jump-tables"="false" "no-nans-fp-math"="true" "no-signed-zeros-fp-math"="true" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="true" "use-soft-float"="false" }
attributes #1 = { noinline norecurse nounwind uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="false" "no-infs-fp-math"="true" "no-jump-tables"="false" "no-nans-fp-math"="true" "no-signed-zeros-fp-math"="true" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="true" "use-soft-float"="false" }
attributes #2 = { nounwind uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="false" "no-infs-fp-math"="true" "no-jump-tables"="false" "no-nans-fp-math"="true" "no-signed-zeros-fp-math"="true" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="true" "use-soft-float"="false" }
attributes #3 = { nounwind }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 7.1.0 "}
!2 = !{!3, !3, i64 0}
!3 = !{!"double", !4, i64 0}
!4 = !{!"omnipotent char", !5, i64 0}
!5 = !{!"Simple C/C++ TBAA"}

; CHECK: define internal {{(dso_local )?}}{ double, double } @diffefunction0(double %y, double %z, double* %x, double* %"x'")
; CHECK-NEXT: entry:
; CHECK-NEXT:   %0 = call { { {}, double*, double* } } @augmented_function(double %y, double %z, double* %x, double* %"x'")
; CHECK-NEXT:   %1 = extractvalue { { {}, double*, double* } } %0, 0
; CHECK-NEXT:   %2 = call {} @diffeaddOne(double* %x, double* %"x'")
; CHECK-NEXT:   %3 = call { double, double } @diffefunction(double %y, double %z, double* %x, double* %"x'", { {}, double*, double* } %1)
; CHECK-NEXT:   ret { double, double } %3
; CHECK-NEXT: }

; CHECK: define internal {{(dso_local )?}}{} @diffeaddOne(double* nocapture %x, double* %"x'")
; CHECK-NEXT: entry:
; CHECK-NEXT:   %0 = load double, double* %x, align 8, !tbaa !2
; CHECK-NEXT:   %add = fadd fast double %0, 1.000000e+00
; CHECK-NEXT:   store double %add, double* %x, align 8, !tbaa !2
; CHECK-NEXT:   %1 = load double, double* %"x'"
; CHECK-NEXT:   store double 0.000000e+00, double* %"x'"
; CHECK-NEXT:   %2 = load double, double* %"x'"
; CHECK-NEXT:   %3 = fadd fast double %2, %1
; CHECK-NEXT:   store double %3, double* %"x'"
; CHECK-NEXT:   ret {} undef
; CHECK-NEXT: }

; CHECK: define internal {{(dso_local )?}}{ {}, double*, double* } @augmented_cast(double* readnone %x, double* %"x'")
; CHECK-NEXT: entry:
; CHECK-NEXT:   %0 = alloca { {}, double*, double* }
; CHECK-NEXT:   %1 = getelementptr { {}, double*, double* }, { {}, double*, double* }* %0, i32 0, i32 1
; CHECK-NEXT:   store double* %x, double** %1
; CHECK-NEXT:   %2 = getelementptr { {}, double*, double* }, { {}, double*, double* }* %0, i32 0, i32 2
; CHECK-NEXT:   store double* %"x'", double** %2
; CHECK-NEXT:   %3 = load { {}, double*, double* }, { {}, double*, double* }* %0
; CHECK-NEXT:   ret { {}, double*, double* } %3
; CHECK-NEXT: }

; CHECK: define internal {{(dso_local )?}}{ { {}, double*, double* } } @augmented_function(double %y, double %z, double* %x, double* %"x'") 
; CHECK-NEXT: entry:
; CHECK-NEXT:   %0 = alloca { { {}, double*, double* } }
; CHECK-NEXT:   %1 = getelementptr { { {}, double*, double* } }, { { {}, double*, double* } }* %0, i32 0, i32 0
; CHECK-NEXT:   %mul = fmul fast double %z, %y
; CHECK-NEXT:   %2 = call { {}, double*, double* } @augmented_cast(double* %x, double* %"x'")
; CHECK-NEXT:   %3 = extractvalue { {}, double*, double* } %2, 1
; CHECK-NEXT:   %4 = getelementptr { {}, double*, double* }, { {}, double*, double* }* %1, i32 0, i32 1
; CHECK-NEXT:   store double* %3, double** %4
; CHECK-NEXT:   %antiptr_call = extractvalue { {}, double*, double* } %2, 2
; CHECK-NEXT:   %5 = getelementptr { {}, double*, double* }, { {}, double*, double* }* %1, i32 0, i32 2
; CHECK-NEXT:   store double* %antiptr_call, double** %5
; CHECK-NEXT:   store double %mul, double* %3, align 8, !tbaa !2
; CHECK-NEXT:   %6 = load { { {}, double*, double* } }, { { {}, double*, double* } }* %0
; CHECK-NEXT:   ret { { {}, double*, double* } } %6
; CHECK-NEXT: }

; CHECK: define internal {{(dso_local )?}}{ double, double } @diffefunction(double %y, double %z, double* %x, double* %"x'", { {}, double*, double* } %tapeArg)
; CHECK-NEXT: entry:
; CHECK-NEXT:   %[[callp:.+]] = extractvalue { {}, double*, double* } %tapeArg, 2
; CHECK-NEXT:   %[[loadcallp:.+]] = load double, double* %[[callp]]
; CHECK-NEXT:   store double 0.000000e+00, double* %[[callp]]
; CHECK-NEXT:   %[[dcast:.+]] = call {} @diffecast(double* %x, double* %"x'", {} undef)
; CHECK-NEXT:   %m0diffez = fmul fast double %[[loadcallp]], %y
; CHECK-NEXT:   %m1diffey = fmul fast double %[[loadcallp]], %z
; CHECK-NEXT:   %[[toret0:.+]] = insertvalue { double, double } undef, double %m1diffey, 0
; CHECK-NEXT:   %[[toret:.+]] = insertvalue { double, double } %[[toret0]], double %m0diffez, 1
; CHECK-NEXT:   ret { double, double } %[[toret]]
; CHECK-NEXT: }

; CHECK: define internal {{(dso_local )?}}{} @diffecast(double* readnone %x, double* %"x'", {} %tapeArg)
; CHECK-NEXT: entry:
; CHECK-NEXT:   ret {} undef
; CHECK-NEXT: }

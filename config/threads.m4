AC_DEFUN(AETOS_FEATURE_THREADS,
[AC_ARG_ENABLE([threads],
[  --disable-threads	Disable the use of threads], [enable_threads=yes])
if test x"$ac_cv_have_pth" = xno || test x"$enable_threads" = xno
then
	AC_DEFINE([NO_THREADS], 1,
[Define to 1 if user disabled threads or if OS has no threads.])
fi])

AC_DEFUN([AETOS_SYS_PREFIX_USCORE],
[AC_CACHE_CHECK([for _ prefix in symbol table],
ac_cv_sys_prefix_uscore,
[ac_cv_sys_prefix_uscore=no
cat > conftest.$ac_ext <<EOF
void foobar_func(){}
int main(){foobar_func();return 42;}
EOF
if AC_TRY_EVAL(ac_compile);
then
	ac_testfile=conftest.$ac_objext
	ac_outfile=conftest.out
	nm $ac_testfile > $ac_outfile
	if egrep '_foobar_func' "$ac_outfile" >/dev/null;
	then
		ac_cv_sys_prefix_uscore=yes
	else
		if egrep 'foobar_func' "$ac_outfile" >/dev/null;
		then
			ac_cv_sys_prefix_uscore=no
		else
			echo "configure: cannot find foobar_func() in $ac_testfile"
		fi
	fi
else
	echo "configure: failed program was:"
	cat conftest.$ac_ext
fi
rm -rf conftest*])
if test x"$ac_cv_sys_prefix_uscore" = xyes;
then
	AC_DEFINE([NEED_USCORE], 1, [Define to 1 if symbol names in libs have a leading underscore. ])
fi])

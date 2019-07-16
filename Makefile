TOP=	.
include ${TOP}/Makefile.config

PROJECT=	"Agar"
PROJECT_GUID=	"93733df2-c743-489e-bc9f-f22aee00d787"
PROJCONFIGDIR=	include/agar/config

include ${TOP}/Makefile.proj

INCDIR=		au core dev gui map math rg sg sk vg
SUBDIR=		core \
		${SUBDIR_gui} \
		${SUBDIR_dev} \
		${SUBDIR_au} \
		${SUBDIR_math} \
		${SUBDIR_sk} \
		${SUBDIR_rg} \
		${SUBDIR_sg} \
		${SUBDIR_vg} \
		${SUBDIR_map}

all: all-subdir
clean: clean-subdir
cleandir: cleandir-config cleandir-subdir
install: install-subdir install-includes install-config
deinstall: deinstall-subdir deinstall-includes deinstall-config
depend: depend-subdir
regress: regress-subdir

includes:
	@if [ ! -e "include" ]; then mkdir include; fi
	@echo "perl mk/gen-includes.pl include/agar"
	@if [ "${SRCDIR}" != "${BLDDIR}" ]; then \
		(cd "${SRCDIR}" && \
		 perl mk/gen-includes.pl "${BLDDIR}/include/agar"); \
	else \
		perl mk/gen-includes.pl include/agar; \
	fi

cleandir-config:
	rm -fR include config 
	rm -f Makefile.config config.log configure.lua .projfiles.out .projfiles2.out
	touch Makefile.config
	-(cd tools && ${MAKE} cleandir)
	find . -name premake.lua -exec rm -f {} \;

release:
	-${MAKE} cleandir
	-${MAKE} all-manlinks
	sh mk/dist.sh stable

install-includes:
	@echo ${INSTALL_INCL_DIR} ${INCLDIR}
	@${SUDO} ${INSTALL_INCL_DIR} ${DESTDIR}${INCLDIR}
	@echo ${INSTALL_INCL_DIR} ${INCLDIR}/agar
	@${SUDO} ${INSTALL_INCL_DIR} ${DESTDIR}${INCLDIR}/agar
	@(cd include/agar && for DIR in ${INCDIR} config; do \
	    echo "${SH} mk/install-includes.sh $$DIR ${INCLDIR}/agar"; \
	    ${SUDO} env \
	      DESTDIR="${DESTDIR}" \
	      INSTALL_INCL_DIR="${INSTALL_INCL_DIR}" \
	      INSTALL_INCL="${INSTALL_INCL}" \
	      ${SH} ${SRCDIR}/mk/install-includes.sh $$DIR ${INCLDIR}/agar; \
	done)
	@for INC in ${INCDIR}; do \
		echo "${INSTALL_INCL} include/agar/$$INC/$${INC}_pub.h ${INCLDIR}/agar/$${INC}.h"; \
		${SUDO} ${INSTALL_INCL} include/agar/$$INC/$${INC}_pub.h ${DESTDIR}${INCLDIR}/agar/$${INC}.h; \
	done
	@echo "${INSTALL_INCL} include/agar/core/web.h ${INCLDIR}/agar/web.h"
	@${SUDO} ${INSTALL_INCL} include/agar/core/web.h ${DESTDIR}${INCLDIR}/agar/web.h

deinstall-includes:
	@echo "rm -rf ${INCLDIR}/agar"
	@${SUDO} rm -rf ${INCLDIR}/agar
	@echo "${DEINSTALL_INCL_DIR} ${INCLDIR}"
	@-${SUDO} ${DEINSTALL_INCL_DIR} ${DESTDIR}${INCLDIR}

install-config:
	@for F in ${AVAIL_CONFIGSCRIPTS}; do \
		echo "${INSTALL_PROG} $$F ${BINDIR}"; \
		${SUDO} ${INSTALL_PROG} $$F ${DESTDIR}${BINDIR}; \
	done
	@if [ "${PKGCONFIG}" != "" ]; then \
		if [ ! -e "${DESTDIR}${PKGCONFIG_LIBDIR}" ]; then \
			echo "${INSTALL_DATA_DIR} ${PKGCONFIG_LIBDIR}"; \
			${SUDO} ${INSTALL_DATA_DIR} ${DESTDIR}${PKGCONFIG_LIBDIR}; \
		fi; \
		for F in ${AVAIL_PCMODULES}; do \
			echo "${INSTALL_DATA} $$F ${PKGCONFIG_LIBDIR}"; \
			${SUDO} ${INSTALL_DATA} $$F ${DESTDIR}${PKGCONFIG_LIBDIR}; \
		done; \
	fi
	@if [ ! -e "${DESTDIR}${PREFIX}/share/aclocal" ]; then \
		echo "${INSTALL_DATA_DIR} ${PREFIX}/share/aclocal"; \
		${SUDO} ${INSTALL_DATA_DIR} ${DESTDIR}${PREFIX}/share/aclocal; \
	fi
	@echo "${INSTALL_DATA} ${SRCDIR}/mk/agar.m4 ${PREFIX}/share/aclocal"
	@${SUDO} ${INSTALL_DATA} ${SRCDIR}/mk/agar.m4 ${DESTDIR}${PREFIX}/share/aclocal

deinstall-config:
	@for F in ${AVAIL_CONFIGSCRIPTS}; do \
		echo "${DEINSTALL_PROG} ${BINDIR}/$$F"; \
		${SUDO} ${DEINSTALL_PROG} ${DESTDIR}${BINDIR}/$$F; \
	done
	@if [ "${PKGCONFIG}" != "" ]; then \
		for F in ${AVAIL_PCMODULES}; do \
			echo "${DEINSTALL_DATA} ${PKGCONFIG_LIBDIR}/$$F"; \
			${SUDO} ${DEINSTALL_DATA} ${DESTDIR}${PKGCONFIG_LIBDIR}/$$F; \
		done; \
	fi
	@echo "${DEINSTALL_DATA} ${PREFIX}/share/aclocal/agar.m4"
	@${SUDO} ${DEINSTALL_DATA} ${DESTDIR}${PREFIX}/share/aclocal/agar.m4

pre-package:
	@if [ "${PKG_OS}" = "windows" ]; then \
		cp -f ${TOP}/mk/install-sdk/install-sdk.exe .; \
		echo '<meta http-equiv="refresh" content="1;url=http://libagar.org/docs/compile-msvc.html" />' > VisualC.html; \
		echo "install-sdk.exe" >> ${PROJFILELIST}; \
		echo "VisualC.html" >> ${PROJFILELIST}; \
		V=`perl mk/get-version.pl`; \
		cat README                       |sed "s/$$/`echo -e \\\r`/" >README.txt; \
		cat INSTALL.txt                  |sed "s/$$/`echo -e \\\r`/" >INSTALL-Windows.txt; \
		cat ChangeLogs/Release-$$V.txt   |sed "s/$$/`echo -e \\\r`/" >RELEASE-$$V.txt; \
		cat LICENSE                      |sed "s/$$/`echo -e \\\r`/" >LICENSE.txt; \
		cat gui/fonts/Vera-Copyright.txt |sed "s/$$/`echo -e \\\r`/" >LICENSE-Vera.txt; \
		cp -f mk/agar-logo.png Logo.png; \
		echo "README.txt"          >> ${PROJFILELIST}; \
		echo "INSTALL-Windows.txt" >> ${PROJFILELIST}; \
		echo "RELEASE-$$V.txt"     >> ${PROJFILELIST}; \
		echo "LICENSE.txt"         >> ${PROJFILELIST}; \
		echo "LICENSE-Vera.txt"    >> ${PROJFILELIST}; \
		echo "Logo.png" >> ${PROJFILELIST}; \
	else \
		V=`perl mk/get-version.pl`; \
		cp ChangeLogs/Release-$$V.txt RELEASE-$$V; \
		cp gui/fonts/Vera-Copyright.txt LICENSE-Vera; \
		cp mk/agar-logo.png Logo.png; \
	fi

post-package:
	@if [ "${PKG_OS}" = "windows" ]; then \
		rm -f install-sdk.exe README.txt INSTALL-Windows.txt VisualC.html; \
		rm -f RELEASE-*.txt LICENSE.txt LICENSE-*.txt Logo.png; \
	else \
		rm -f Release-* ChangeLog-* LICENSE-* Logo.png; \
	fi

function-list:
	find . -name \*.3 -exec grep ^\.Fn {} \; |awk '{print $$2}' |uniq

.PHONY: clean cleandir install deinstall depend regress includes
.PHONY: cleandir-config release
.PHONY: install-includes deinstall-includes install-config deinstall-config
.PHONY: pre-package post-package function-list

include ${TOP}/mk/build.common.mk
include ${TOP}/mk/build.subdir.mk
include ${TOP}/mk/build.man.mk
include ${TOP}/mk/build.proj.mk

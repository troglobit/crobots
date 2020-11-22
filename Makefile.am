SUBDIRS        = doc examples src
dist_doc_DATA  = README.md LICENSE
dist_man6_MANS = crobots.6

# Add --show-overrides to see what overrides are in place, or have a
# look in the file debian/source/linitian-overrides
package:
	@debuild -uc -us -B --lintian-opts --profile debian -i -I

## Check if tagged in git
release-hook:
	if [ "x`git tag -l v$(PACKAGE_VERSION)|grep $(PACKAGE_VERSION)`" = "x" ]; then	\
		echo;									\
		printf "\e[1m\e[41mCannot find release tag $(PACKAGE_VERSION)\e[0m\n";	\
		printf "\e[1m\e[5mDo release anyway?\e[0m "; read yorn;			\
		if [ "$$yorn" != "y" -a "$$yorn" != "Y" ]; then				\
			printf "OK, aborting release.\n";				\
			exit 1;								\
		fi;									\
		echo;									\
	else										\
		echo;									\
		printf "\e[1m\e[42mFound GIT release tag $(PACKAGE_VERSION)\e[0m\n";	\
		printf "\e[1m\e[44m>>Remember to push tags!\e[0m\n";			\
		echo;									\
	fi

## Target to run when building a release
release: release-hook distcheck package
	@for file in $(DIST_ARCHIVES); do	\
		md5sum $$file > ../$$file.md5;	\
	done
	@mv $(DIST_ARCHIVES) ../
	@echo
	@echo "Resulting release files in ../"
	@echo "================================================================="
	@for file in $(DIST_ARCHIVES); do						\
		printf "%-32s Distribution tarball\n" $$file;				\
		printf "%-32s " $$file.md5; cat ../$$file.md5 | cut -f1 -d' ';		\
	done
	@for file in `cd ..; ls crobots_$(VERSION)* $(PACKAGE)-dbg*_$(VERSION)*`; do	\
		printf "%-32s Debian/Ubuntu package\n" $$file;				\
	done

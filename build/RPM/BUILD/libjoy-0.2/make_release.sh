#! /bin/bash

### pars: ftp_user ftp_pw changes
### will prepare rpmmacros
### and rpm dir structure if neeeded
### will check for debuild ,gcc,rpmbuild
### will build libjoy
### creates src pkg , deb and rpms 
### uploads stuff to voyagerproject.org if everything worked fine
### updates article / or use directory content view
### updates the freshmeat page

FTP_HOST="ftp.voyagerproject.org"

FTP_USER=$1
FTP_PASSWORD=$2
CHANGES=$3

#checking parameters
if [ -z "$FTP_USER" ]; then
echo "ERROR: no freshmeat user name given"
exit 1
fi
if [ -z "$FTP_PASSWORD" ]; then
echo "ERROR: no freshmeat password name given"
exit 1
fi
if [ -z "$CHANGES" ]; then
echo "ERROR: no changes given"
exit 1
fi


#getting build directory
ROOT=`pwd`


#getting version info from project file
VERSION=`cat libjoy.kdevelop | grep -m 1 "<version>" | cut -d'>' -f2 | cut -d'<' -f1`
NAME=`cat libjoy.kdevelop | grep "<projectname>" | cut -d'>' -f2 | cut -d'<' -f1`
#multi line descriptions will fail
DESCRIPTION=`cat libjoy.kdevelop | grep -m 1 "<description>" | cut -d'>' -f2 | cut -d'<' -f1`
AUTHOR=`cat libjoy.kdevelop | grep -m 1 "<author>" | cut -d'>' -f2 | cut -d'<' -f1`


#the build root for rpms
BUILDROOT="/build"


#checking if build root exists
if [ -e "$ROOT$BUILDROOT" ]; then
echo "found existing build directory"
else
echo "found no build directory ,creating one"
mkdir "$ROOT$BUILDROOT"
fi


#checking if rpm tree exists & creating it if not there
if [ -e "$ROOT$BUILDROOT/RPM" ]; then
echo "found existing RPM tree"
if [ -e "$ROOT$BUILDROOT/RPM/BUILD" ]; then
echo "found existing RPM/BUILD directory"
else
echo "found no RPM/BUILD directory ,creating one"
mkdir "$ROOT$BUILDROOT/RPM/BUILD"
fi
if [ -e "$ROOT$BUILDROOT/RPM/RPMS" ]; then
echo "found existing RPM/RPMS directory"
else
echo "found no RPM/RPMS directory ,creating one"
mkdir "$ROOT$BUILDROOT/RPM/RPMS"
fi
if [ -e "$ROOT$BUILDROOT/RPM/SOURCES" ]; then
echo "found existing RPM/SOURCES directory"
else
echo "found no RPM/SOURCES directory ,creating one"
mkdir "$ROOT$BUILDROOT/RPM/SOURCES"
fi
if [ -e "$ROOT$BUILDROOT/RPM/SPECS" ]; then
echo "found existing RPM/SPECS directory"
else
echo "found no RPM/SPECS directory ,creating one"
mkdir "$ROOT$BUILDROOT/RPM/SPECS"
fi
if [ -e "$ROOT$BUILDROOT/RPM/SRPMS" ]; then
echo "found existing RPM/SRPMS directory"
else
echo "found no RPM/SRPMS directory ,creating one"
mkdir "$ROOT$BUILDROOT/RPM/SRPMS"
fi
#maybe not needed
if [ -e "$ROOT$BUILDROOT/RPM/tmp" ]; then
echo "found existing RPM/tmp directory"
else
echo "found no RPM/tmp directory ,creating one"
mkdir "$ROOT$BUILDROOT/RPM/tmp"
fi
else
echo "found no RPM tree ,building one"
mkdir "$ROOT$BUILDROOT/RPM"
mkdir "$ROOT$BUILDROOT/RPM/BUILD"
mkdir "$ROOT$BUILDROOT/RPM/RPMS"
mkdir "$ROOT$BUILDROOT/RPM/SOURCES"
mkdir "$ROOT$BUILDROOT/RPM/SPECS"
mkdir "$ROOT$BUILDROOT/RPM/SRPMS"
mkdir "$ROOT$BUILDROOT/RPM/tmp"
fi


#checking if rpmmacros is there (backup & update to BUILDROOT/RPM)
if [ -e ~/.rpmmacros ]; then
echo "found existing .rpmmacros file in your home,backing it up"
mv ~/.rpmmacros ~/.rpmmacros.build.tmp
fi


#creating new .rpmmacros file
echo "%_topdir $ROOT$BUILDROOT/RPM" >> ~/.rpmmacros


#updating package creation files
#(most important part of the script)
#update libjoy.spec
sed -e "s/Version: [.0123456789]*$/Version: $VERSION/g" "$ROOT/$NAME.spec"  > "$ROOT/$NAME.spec.tmp"
mv "$ROOT/$NAME.spec.tmp" "$ROOT/$NAME.spec"

#add change log entry with actual date
CHANGEDATE=`LANG=C date +"%a %b %d %Y"`
#echo "* $CHANGEDATE Tim Theede <pez2001@voyagerproject.de>" >> "$ROOT/$NAME.spec"
#multiline changes may break things here
#echo "- $CHANGES" >> "$ROOT/$NAME.spec"
CHANGELINE="\n* $CHANGEDATE Tim Theede <pez2001@voyagerproject.de>\n- $CHANGES"
sed -e "s/^%changelog/%changelog $CHANGELINE/g" "$ROOT/$NAME.spec" > "$ROOT/$NAME.spec.tmp"
mv "$ROOT/$NAME.spec.tmp" "$ROOT/$NAME.spec"
#update debian/...
CHANGEDATE2=`LANG=C date -R`
mv "$ROOT/debian/changelog" "$ROOT/debian/changelog.tmp"
echo "$NAME ($VERSION) stable; urgency=high" >> "$ROOT/debian/changelog"
echo "" >> "$ROOT/debian/changelog"
echo "  * $CHANGES" >> "$ROOT/debian/changelog"
echo "" >> "$ROOT/debian/changelog"
echo " -- Tim Theede <pez2001@voyagerproject.de>  $CHANGEDATE2" >> "$ROOT/debian/changelog"
echo "" >> "$ROOT/debian/changelog"
cat "$ROOT/debian/changelog.tmp" >> "$ROOT/debian/changelog"
rm "$ROOT/debian/changelog.tmp"

#create source.tar.gz
SOURCEFILE_BASE="$NAME-$VERSION"
#copy all needed files into build/src
SRCROOT="$BUILDROOT/src"
SRCDIR="$ROOT$SRCROOT/$NAME-$VERSION"
#checking dirs
if [ -e "$ROOT$SRCROOT" ]; then
echo "found existing $ROOT$SRCROOT directory"
rm -R $ROOT$SRCROOT
echo "removed existing $ROOT$SRCROOT directory"
mkdir $ROOT$SRCROOT
echo "created new $ROOT$SRCROOT directory"
else
echo "found no src root directory ,creating one"
mkdir "$ROOT$SRCROOT"
fi
if [ -e "$SRCDIR" ]; then
echo "found existing $SRCDIR directory"
rm -R $SRCDIR
echo "removed existing $SRCDIR directory"
mkdir $SRCDIR
echo "created new $SRCDIR directory"
else
echo "found no $SRCDIR directory ,creating one"
mkdir "$SRCDIR"
fi
#create subdirs
mkdir "$SRCDIR/src"
mkdir "$SRCDIR/debian"
mkdir "$SRCDIR/latex"
#copy files
cp "src/libjoy.c" "$SRCDIR/src"
cp "src/libjoy.h" "$SRCDIR/src"
cp "src/libjoytest.c" "$SRCDIR/src"
cp "src/joytool.c" "$SRCDIR/src"
cp "src/joytool.h" "$SRCDIR/src"
cp "src/CMakeLists.txt" "$SRCDIR/src"
cp "src/Makefile" "$SRCDIR/src"
cp "readme" "$SRCDIR/"
cp "libjoy.spec" "$SRCDIR/"
cp "COPYING" "$SRCDIR/"
cp "CMakeLists.txt" "$SRCDIR/"
cp "Makefile" "$SRCDIR/"
cp "Doxyfile" "$SRCDIR/"
cp "libjoy.kdevelop" "$SRCDIR/"
cp "make_release.sh" "$SRCDIR/"
cp "debian/changelog" "$SRCDIR/debian"
cp "debian/compat" "$SRCDIR/debian"
cp "debian/control" "$SRCDIR/debian"
cp "debian/copyright" "$SRCDIR/debian"
#cp "debian/dirs" "$SRCDIR/debian"
cp "debian/docs" "$SRCDIR/debian"
#cp "debian/files" "$SRCDIR/debian"
cp "debian/rules" "$SRCDIR/debian"
cp "latex/Makefile" "$SRCDIR/latex"
#tar and gzip files
SOURCEFILE="$SOURCEFILE_BASE.tar.gz"
cd "$ROOT$SRCROOT"
tar -cf - "$NAME-$VERSION" | gzip -9 > "$ROOT/$SOURCEFILE"
echo "created src package"
cd "$ROOT"

#copy source tar.gz to BUILDROOT/RPM/SOURCES
cp "$ROOT/$SOURCEFILE" "$ROOT$BUILDROOT/RPM/SOURCES/$SOURCEFILE"


#copy spec to BUILDROOT/RPM/SPECS
cp libjoy.spec "$ROOT$BUILDROOT/RPM/SPECS/libjoy.spec"


#the releases root directory
RELEASESROOT="/releases"


#check if releases directory exists
if [ -e "$ROOT$RELEASESROOT" ]; then
echo "found existing releases directory"
else
echo "found no releases directory ,creating one"
mkdir "$ROOT$RELEASESROOT"
fi


#create release directory releases/VERSION
if [ -e "$ROOT$RELEASESROOT/$VERSION" ]; then
echo "ERROR: found existing $ROOT$RELEASESROOT/$VERSION directory"
exit 1
else
echo "found no $ROOT$RELEASESROOT/$VERSION directory ,creating one"
mkdir "$ROOT$RELEASESROOT/$VERSION"
fi

#build makefile
cd "$SRCDIR"
cmake .
cd "$ROOT"


#compile library and joytool
cd "$SRCDIR"
make
cd "$ROOT"


#create debian and rpm packages
cd "$SRCDIR"
debuild -us -uc
cd "$ROOT"
echo "created debian package"
rpmbuild -ba "$NAME.spec"
echo "created rpm package"

#move packages to release dir
mv "$ROOT$BUILDROOT/RPM/RPMS/i386/$NAME-$VERSION-1.i386.rpm" "$ROOT$RELEASESROOT/$VERSION/"
mv "$ROOT$BUILDROOT/RPM/SRPMS/$NAME-$VERSION-1.src.rpm" "$ROOT$RELEASESROOT/$VERSION/"
DEBSUFFIX="_$VERSION"
DEBSUFFIX2="_i386"
mv "$ROOT$BUILDROOT/src/$NAME$DEBSUFFIX.dsc" "$ROOT$RELEASESROOT/$VERSION/"
mv "$ROOT$BUILDROOT/src/$NAME$DEBSUFFIX$DEBSUFFIX2.changes" "$ROOT$RELEASESROOT/$VERSION/"
mv "$ROOT$BUILDROOT/src/$NAME$DEBSUFFIX$DEBSUFFIX2.deb" "$ROOT$RELEASESROOT/$VERSION/"
mv "$ROOT/$SOURCEFILE" "$ROOT$RELEASESROOT/$VERSION/"
echo "moved release files to $ROOT$RELEASEROOT/$VERSION/"

#upload files to voyagerproject.org
ncftpput -p "$FTP_PASSWORD" -u "$FTP_USER" -C "$FTP_HOST" "$ROOT$RELEASESROOT/$VERSION/$NAME-$VERSION-1.i386.rpm" "/var/www/html/gpl/libjoy/$NAME-$VERSION-1.i386.rpm"
ncftpput -p "$FTP_PASSWORD" -u "$FTP_USER" -C "$FTP_HOST" "$ROOT$RELEASESROOT/$VERSION/$NAME-$VERSION-1.src.rpm" "/var/www/html/gpl/libjoy/$NAME-$VERSION-1.src.rpm"
ncftpput -p "$FTP_PASSWORD" -u "$FTP_USER" -C "$FTP_HOST" "$ROOT$RELEASESROOT/$VERSION/$NAME$DEBSUFFIX.dsc" "/var/www/html/gpl/libjoy/$NAME$DEBSUFFIX.dsc"
ncftpput -p "$FTP_PASSWORD" -u "$FTP_USER" -C "$FTP_HOST" "$ROOT$RELEASESROOT/$VERSION/$NAME$DEBSUFFIX$DEBSUFFIX2.changes" "/var/www/html/gpl/libjoy/$NAME$DEBSUFFIX$DEBSUFFIX2.changes"
ncftpput -p "$FTP_PASSWORD" -u "$FTP_USER" -C "$FTP_HOST" "$ROOT$RELEASESROOT/$VERSION/$NAME$DEBSUFFIX$DEBSUFFIX2.deb" "/var/www/html/gpl/libjoy/$NAME$DEBSUFFIX$DEBSUFFIX2.deb"
ncftpput -p "$FTP_PASSWORD" -u "$FTP_USER" -C "$FTP_HOST" "$ROOT$RELEASESROOT/$VERSION/$SOURCEFILE" "/var/www/html/gpl/libjoy/$SOURCEFILE"
echo "uploaded packages to voyagerproject.org"

#update freshmeat page
echo "Project: $NAME" > "$ROOT/fsc.tmp"
echo "Version: $VERSION" >> "$ROOT/fsc.tmp"
echo "Release-Focus: Code cleanup" >> "$ROOT/fsc.tmp"
echo "Hide: Y" >> "$ROOT/fsc.tmp"
echo "Home-Page-URL: http://www.voyagerproject.org/projects/33-projects/62-ljoy" >> "$ROOT/fsc.tmp"
echo "Gzipped-Tar-URL: http://www.voyagerproject.org/gpl/libjoy/$NAME-$VERSION.tar.gz" >> "$ROOT/fsc.tmp"
echo "Changelog-URL: http://www.voyagerproject.org/gpl/libjoy/$NAME$DEBSUFFIX$DEBSUFFIX2.changes" >> "$ROOT/fsc.tmp"
echo "RPM-URL: http://www.voyagerproject.org/gpl/libjoy/$NAME-$VERSION-1.i386.rpm" >> "$ROOT/fsc.tmp"
echo "Debian-URL: http://www.voyagerproject.org/gpl/libjoy/$NAME$DEBSUFFIX$DEBSUFFIX2.deb" >> "$ROOT/fsc.tmp"
echo "" >> "$ROOT/fsc.tmp"
echo "$CHANGES" >> "$ROOT/fsc.tmp"
freshmeat-submit << "$ROOT/fsc.tmp"
rm "$ROOT/fsc.tmp"
echo "updated freshmeat entry"


#TODO 
#create dev-blog entry for release


#update subversion tree


#moving old rpmmmacros file back if existing
if [ -e ~/.rpmmacros.build.tmp ]; then
echo "found backup .rpmmacros file in your home,restoring it up"
if [ -e ~/.rpmmacros ]; then
rm ~/.rpmmacros
fi
mv ~/.rpmmacros.build.tmp ~/.rpmmacros
fi

Name:           rancid
Version:        2.3.6
Release:        1%{?dist}
Summary:        Really Awesome New Cisco confIg Differ

Group:          Applications/System
License:        non-free
URL:            http://www.shrubbery.net/rancid/
Source:         %{name}-%{version}.tar.gz
BuildRoot:      %{_tmppath}/%{name}-%{version}-root
Requires:       cvs 
BuildRequires:  expect >= 5.40

%package lg
Summary:        RANCID Looking Glass CGI scripts
Group:          Applications/System

%description
Rancid is a "Really Awesome New Cisco confIg Differ" developed to
maintain CVS controlled copies of router configs. Rancid is not limited
to Cisco devices. It currently supports Cisco routers, Juniper routers,
Catalyst switches, Foundry switches, Redback NASs, ADC EZT3 muxes, MRTd
(and thus likely IRRd), Alteon switches, and HP procurve switches and a
host of others.

%description lg
RANCID also includes looking glass software. It is based on Ed Kern's
looking glass which was once used for http://nitrous.digex.net/, for the
old-school folks who remember it. Our version has added functions, supports
cisco, juniper, and foundry and uses the login scripts that come with
rancid; so it can use telnet or ssh to connect to your devices(s).

%prep
%setup -q

%build
%configure --localstatedir=%{_localstatedir}/rancid
make

%install
rm -rf $RPM_BUILD_ROOT
#Fix the missing statedir
install -m 755 -d $RPM_BUILD_ROOT/%{_localstatedir}/rancid
make install DESTDIR=$RPM_BUILD_ROOT
# Get rid of unwanted /usr/share/rancid install
rm -rf $RPM_BUILD_ROOT/%{_datadir}/rancid
# Move lg CGI scripts to CGI directory
install -m 755 -d  $RPM_BUILD_ROOT/var/www/cgi-bin
mv $RPM_BUILD_ROOT/%{_bindir}/*.cgi $RPM_BUILD_ROOT/var/www/cgi-bin
# Workaround for the stupid rpmbuild to NOT search for dependencies in the
# documentation. We need to do it here as %doc ignores %attr.
find share -type f -print | xargs chmod a-x
# Install the sample .cloginrc file
cp cloginrc.sample $RPM_BUILD_ROOT/%{_localstatedir}/rancid/.cloginrc

%pre
if [ $1 -eq 1 ]; then
   egrep -q '^rancid:' /etc/passwd || useradd -M -r -d %{_localstatedir}/rancid -c "RANCID User" rancid
fi

%postun
if [ $1 -eq 0 ]; then
   # It's a matter of taste if we should remove the user on uninstall or not
   userdel rancid
fi

%clean
rm -rf $RPM_BUILD_ROOT


%files
%defattr(-,root,root,0755)
%doc BUGS CHANGES COPYING FAQ README UPGRADING Todo
%doc share/cisco-load.exp share/cisco-reload.exp
%doc share/downreport share/getipacctg share/rtrfilter
%config(noreplace) /etc/rancid.conf
%{_bindir}/*
%{_mandir}/man1/[a-k]*
%{_mandir}/man1/[m-z]*
%{_mandir}/man5/[a-k]*
%{_mandir}/man5/[m-z]*
%dir %attr(770,rancid,rancid) %{_localstatedir}/rancid
%config(noreplace) %attr(640,rancid,rancid) %{_localstatedir}/rancid/.cloginrc

%files lg
%defattr(-,root,root,0755)
%config(noreplace) /etc/lg.conf
%{_mandir}/man1/lg_intro*
%{_mandir}/man5/lg.conf*
/var/www/cgi-bin/*
%doc README.lg

%changelog
* Fri Feb 11 2011 Florian Koch <fkoch@xxxxxxxx> 2.3.6
- Modified Version to be 2.3.6
- Fix missing 'expect' as BuildRequire (was only Require)
- Fix creation of missing statedir before make install
- Fix some typos
- Replace mkdir with install
- Make rpmlint clean (add %%defattr() to lg files section)

* Mon Jul 19 2010 Lance Vermilion <rancid@xxxxxxxx> 2.3.4
- Modified Version to be 2.3.4 and Release to be 1%%{?dist} instead of 2%%{?dist}

* Fri Feb 15 2008 Steve Snodgrass <ssnodgra@xxxxxxxx> 2.3.2a8-1
- Install .cloginrc as a configuration file
- Don't try to create the rancid user if it already exists

* Wed Feb 13 2008 Steve Snodgrass <ssnodgra@xxxxxxxx> 2.3.2a8-1
- Create subpackage for looking glass CGI scripts
- Include configuration files in RPM
- Many other tweaks

* Wed Nov 16 2005 Michael Stefaniuc <mstefani@xxxxxxxxxx> 2.3.1-3
- Use /var/rancid as localstatedir
- Create the rancid user on install and remove it on uninstall
- Use %%doc correctly

* Wed Nov 02 2005 Michael Stefaniuc <mstefani@xxxxxxxxxx> 2.3.1-2
- Original spec file by Dan Pfleger.
- Add a changelog.
- Make the formating of the spec file adhere to the Fedora Extras Packaging
 guidelines.
- New %%description based on the README and the website.
- Add cvs Requires.
- Changed Group
- Use macros in the files section. Simplify it.
- Do not install the looking glass cgi's. Those make rpm pull in more perl
 module dependencies.

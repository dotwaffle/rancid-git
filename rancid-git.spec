Name:    rancid-git
Version: 2.3.8
Release: 0%{?dist}
Summary: Really Awesome New Cisco confIg Differ (w/ git support)

Group:   Applications/Internet
License: BSD with advertising
URL:     https://github.com/dotwaffle/rancid-git
Source:  http://github.com/dotwaffle/rancid-git/tarball/master

BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)

BuildRequires: telnet
BuildRequires: rsh
BuildRequires: openssh-clients
BuildRequires: expect >= 5.40
BuildRequires: cvs
BuildRequires: subversion
BuildRequires: git
BuildRequires: perl
BuildRequires: iputils
BuildRequires: automake
BuildRequires: libtool
BuildRequires: sysconftool

Conflicts: rancid

Requires: shadow-utils
Requires: findutils
Requires: expect >= 5.40
Requires: perl
Requires: iputils
Requires: logrotate

%description
RANCID monitors a router's (or more generally a device's) configuration,
including software and hardware (cards, serial numbers, etc) and uses CVS
(Concurrent Version System), Subversion, or git to maintain history of changes.


%prep
%setup -q -n %{name}-%{release}

%build
aclocal; autoheader; automake; autoconf
%configure --sysconfdir=%{_sysconfdir}/rancid --bindir=%{_libexecdir}/rancid --enable-conf-install
make %{?_smp_mflags}


%install
rm -rf %{buildroot}
make install DESTDIR=%{buildroot} INSTALL="install -p"
install -d -m 0755 %{buildroot}/%{_localstatedir}/rancid
install -d -m 0755 %{buildroot}/%{_localstatedir}/log/rancid
install -d -m 0755 %{buildroot}/%{_localstatedir}/log/rancid/old
install -d -m 0755 %{buildroot}/%{_bindir}/

#symlink some bins from _libexecdir/rancid to _bindir
for base in \
 rancid rancid-cvs rancid-fe rancid-run
 do
 ln -sf %{_libexecdir}/rancid/${base} \
  %{buildroot}/%{_bindir}/${base}
done


%clean
rm -rf %{buildroot}

%pre
getent group rancid >/dev/null || groupadd -r rancid
getent passwd rancid >/dev/null || \
useradd -r -g rancid -d %{_localstatedir}/rancid/ -s /bin/bash \
-k /etc/skel -m -c "RANCID" rancid
exit 0

%postun
getent passwd rancid >/dev/null && userdel rancid
getent group rancid >/dev/null && groupdel rancid
if [ -d /var/rancid ]; then rm -rf /var/rancid; fi

%files
%defattr(-,root,root,-)
%doc CHANGES cloginrc.sample COPYING FAQ README README.lg Todo

#%%{_sysconfdir}-files
%attr(0750,root,rancid) %dir %{_sysconfdir}/rancid
%attr(0640,root,rancid) %config(noreplace) %{_sysconfdir}/rancid/*

#_libexecdir/rancid-files
%dir %{_libexecdir}/rancid/
%{_libexecdir}/rancid/*

#_bindir-files
%{_bindir}/*

#_mandir-files
%{_mandir}/*/*

#_datadir/rancid-files
%dir %{_datadir}/rancid/
%{_datadir}/rancid/*

#_localstatedir-directories
%attr(0750,root,rancid) %dir %{_localstatedir}/log/rancid
%attr(0750,root,rancid) %dir %{_localstatedir}/log/rancid/old
%attr(0750,root,rancid) %dir %{_localstatedir}/rancid/


%changelog
* Wed Jun 05 2013 Paul Morgan <jumanjiman@gmail.com> 2.3.8-0
- use tito to build rpm
- fix rpm spec file

* Wed Dec 19 2012 Jonathan Thurman <jthurman@newrelic.com> 2.3.8-2
- Patch to ignore .dat files of flash

* Mon Dec 17 2012 Jonathan Thurman <jthurman@newrelic.com> 2.3.8-1
- Rebuild with git support from upstream 2.3.8 release

* Sun Jan 23 2011 Peter Robinson <pbrobinson@gmail.com> 2.3.6-1
- New upstream 2.3.6 release

* Tue Sep 28 2010 Peter Robinson <pbrobinson@gmail.com> 2.3.4-1
- New upstream 2.3.4 release

* Wed Jul 22 2009 Gary T. Giesen <giesen@snickers.org> 2.3.2-3
- Changed GECOS name for rancid user

* Wed Jul 22 2009 Gary T. Giesen <giesen@snickers.org> 2.3.2-2
- Added logrotate (and updated crontab to let logrotate handle log file
  cleanup
- Removed Requires: for rsh, telnet, and openssh-clients
- Removed Requires: for cvs
- Cleaned up file permissions
- Added shell for rancid user for CVS tree creation and troubleshooting
- Patch cron file for installation path
- Removed installation of CVS root to permit SVN use
- Moved from libdir to libexecdir

* Thu Jul 16 2009 Gary T. Giesen <giesen@snickers.org> 2.3.2-1
- Updated to 2.3.2 stable
- Removed versioned expect requirement so all supported Fedora/EPEL releases
  now meet the minimum
- Spec file cleanup/style changes

* Wed Oct 08 2008 Aage Olai Johnsen <aage@thaumaturge.org> 2.3.2-0.6a8
- Some fixes (#451189)

* Tue Sep 30 2008 Aage Olai Johnsen <aage@thaumaturge.org> 2.3.2-0.5a8
- Some fixes (#451189)

* Tue Sep 30 2008 Aage Olai Johnsen <aage@thaumaturge.org> 2.3.2-0.4a8
- More fixes (#451189)
- Patched Makefiles - Supplied by Mamoru Tasaka (mtasaka@ioa.s.u-tokyo.ac.jp)

* Tue Sep 23 2008 Aage Olai Johnsen <aage@thaumaturge.org> 2.3.2-0.3a8
- More fixes (#451189)

* Wed Jul 09 2008 Aage Olai Johnsen <aage@thaumaturge.org> 2.3.2a8-0.2a8
- Plenty of fixes (#451189)
- Patched rancid.conf-file
- Added cronjob

* Sat May 31 2008 Aage Olai Johnsen <aage@thaumaturge.org> 2.3.2a8-0.1
- Initial RPM release

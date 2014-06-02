Name:		rofl
Version:	0.4.1
Release:	0%{?dist}
Summary:	revised openflow library
Epoch:		0

Group:		System Environment/Libraries
License:	Mozilla Public License Version 2.0, http://www.mozilla.org/MPL/2.0
URL:		https://github.com/bisdn/rofl-core
Source0:	rofl-%{version}.tar.gz

BuildRequires:	libconfig-devel openssl-devel
Buildroot: 	%{_tmppath}/%{name}-%{version}-root 
Requires:	libconfig openssl

%description
Revised OpenFlow Library (ROFL) version v0.4.1

%package        devel
Summary:        Header files and libraries for revised openflow library
Group:          Development/Libraries
Requires:       %{name} = %{version}-%{release}

%description    devel
Revised OpenFlow Library (ROFL) version v0.4.1 development files


%prep
%setup 


%build
sh autogen.sh
cd build/
../configure --prefix=/usr/local --disable-silent-rules
make %{?_smp_mflags}


%install
rm -rf $RPM_BUILD_ROOT
cd build/
make install DESTDIR=$RPM_BUILD_ROOT


%post
/sbin/ldconfig
/bin/ln -s /usr/local/lib/librofl.so.0 /usr/local/lib/librofl.so
/bin/ln -s /usr/local/lib/librofl_hal.so.0 /usr/local/lib/librofl_hal.so
/bin/ln -s /usr/local/lib/librofl_pipeline.so.0 /usr/local/lib/librofl_pipeline.so
%postun
/sbin/ldconfig
/bin/rm /usr/local/lib/librofl.so
/bin/rm /usr/local/lib/librofl_hal.so
/bin/rm /usr/local/lib/librofl_pipeline.so


%define _unpackaged_files_terminate_build 0 

%files
%defattr(-,root,root,-)
/usr/local/lib/librofl.so.0.1.1
/usr/local/lib/librofl_hal.so.0.0.0
/usr/local/lib/librofl_pipeline.so.0.0.0
/usr/local/lib/librofl.a
/usr/local/lib/librofl.la
/usr/local/lib/librofl_hal.a
/usr/local/lib/librofl_hal.la
/usr/local/lib/librofl_pipeline.a
/usr/local/lib/librofl_pipeline.la
/usr/local/sbin/ethswctld
%doc


%files devel
%defattr(-,root,root,-)
/usr/local/include/rofl.h
/usr/local/include/rofl_config.h
/usr/local/include/rofl
/usr/local/lib/pkgconfig/rofl.pc
%doc


%clean
rm -rf $RPM_BUILD_ROOT 

%changelog
* Mon May 26 2014 Andreas Koepsel <andreas.koepsel@bisdn.de>
- build package for rofl-0.4.0
* Fri Apr 25 2014 Andreas Koepsel <andreas.koepsel@bisdn.de>
- build package for rofl-0.4rc2


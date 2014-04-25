Name:		rofl
Version:	0.4.0
Release:	rc2%{?dist}
Summary:	revised openflow library

Group:		System Environment/Libraries
License:	Mozilla Public License Version 2.0, http://www.mozilla.org/MPL/2.0/
URL:		http://codebasin.net/rofl-core
Source0:	rofl-%{version}.tar.gz

BuildRequires:	libconfig-devel openssl-devel
Buildroot: 	%{_tmppath}/%{name}-%{version}-root 
Requires:	libconfig openssl

%description
Revised OpenFlow Library (ROFL) version v0.4rc2

%package        devel
Summary:        Header files and libraries for revised openflow library
Group:          Development/Libraries
Requires:       %{name} = %{version}-%{release}

%description    devel
Revised OpenFlow Library (ROFL) version v0.4rc2 development files


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
%postun
/sbin/ldconfig


%define _unpackaged_files_terminate_build 0 

%files
%defattr(-,root,root,-)
/usr/local/lib/librofl_hal.so.0.0.0
/usr/local/lib/librofl_pipeline.so.0.0.0
/usr/local/lib/librofl.so.0.1.1
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
* Fri Apr 25 2014 Andreas Koepsel <andreas.koepsel@bisdn.de>
- build package for rofl-0.4rc2


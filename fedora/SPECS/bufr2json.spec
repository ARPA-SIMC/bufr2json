%global releaseno 1
# Note: define _srcarchivename in Travis build only.
%{!?srcarchivename: %global srcarchivename %{name}-%{version}-%{releaseno}}
# Python 3 package names
# Python 3 package names
%if 0%{?rhel} == 7
%define python3_vers python36
%else
%define python3_vers python3
%endif

Name:           bufr2json
Version:        0.20
Release:        %{releaseno}%{?dist}
Summary:        BUFR to JSON converter
License:        GPLv2+
URL:            https://github.com/arpa-simc/%{name}
Source0:        https://github.com/arpa-simc/%{name}/archive/v%{version}-%{releaseno}.tar.gz#/%{srcarchivename}.tar.gz
BuildArch:      noarch
BuildRequires:  autoconf
BuildRequires:  automake
BuildRequires:  make
BuildRequires:  %{python3_vers}
BuildRequires:  %{python3_vers}-dballe >= 8.0
Requires:       %{python3_vers}
Requires:       %{python3_vers}-dballe >= 8.0

%description
BUFR to JSON converter


%prep
%setup -q -n %{srcarchivename}


%build
autoreconf -ifv
%configure
make %{?_smp_mflags}


%install
rm -rf $RPM_BUILD_ROOT
%make_install

%check
make check


%files
%doc
%{_bindir}/bufr2json

%changelog
* Wed Jun  7 2023 Emanuele Di Giacomo <edigiacomo@arpae.it> - 0.20-1
- New version to reflect upstream

* Tue May 11 2021 Daniele Branchini <dbranchini@arpae.it> - 0.19-2
- Added missing dependency

* Mon Feb  3 2020 Emanuele Di Giacomo <edigiacomo@arpae.it> - 0.19-1
- Fixed coordinates

* Wed Jun 12 2019 Emanuele Di Giacomo <edigiacomo@arpae.it> - 0.18-1
- Use cursor-based iteration
- Test suite

* Thu Jun  6 2019 Emanuele Di Giacomo <edigiacomo@arpae.it> - 0.17-3
- python36 for CentOS

* Wed Mar 27 2019 Daniele Branchini <dbranchini@arpae.it> - 0.17-2
- Added missing dependency

* Fri Mar  1 2019 Emanuele Di Giacomo <edigiacomo@arpae.it> - 0.17-1
- Optimizations

* Thu Feb 28 2019 Emanuele Di Giacomo <edigiacomo@arpae.it> - 0.16-1
- dballe 8 support
- Python rewrite
- Removed dballe json support

* Wed Jun  6 2018 Emanuele Di Giacomo <edigiacomo@arpae.it> - 0.15-2
- Updated spec file

* Mon Dec 12 2016 Emanuele Di Giacomo <edigiacomo@arpae.it> - 0.15-1
- dballe 7.21 support

* Tue Sep  8 2015 Emanuele Di Giacomo <edigiacomo@arpae.it> - 0.14-1
- Minor changes

* Fri Sep  4 2015 Emanuele Di Giacomo <edigiacomo@arpae.it> - 0.13-1
- Removed options
- Dballe format

* Mon May 25 2015 Emanuele Di Giacomo <edigiacomo@arpae.it> - 0.10-1
- Fixed for dballe API > 7.1-4737

* Wed May 20 2015 Emanuele Di Giacomo <edigiacomo@arpae.it> - 0.9-1
- Added --attributes option

* Tue Feb 25 2014 Emanuele Di Giacomo <edigiacomo@arpae.it> - 0.8-1
- "ident" property

* Wed Feb 19 2014 Emanuele Di Giacomo <edigiacomo@arpae.it> - 0.7.1-1
- Optional geohash size
- Fixed segfault

* Thu Feb 06 2014 Emanuele Di Giacomo <edigiacomo@arpae.it> - 0.6-1
- "lon" and "lat" properties

* Fri Jul 05 2013 Emanuele Di Giacomo <edigiacomo@arpae.it> - 0.5-1
- Skip invalid data

* Fri Jul 05 2013 Emanuele Di Giacomo <edigiacomo@arpae.it> - 0.4-1
- Data attributes and geohash support

* Tue Jul 02 2013 Emanuele Di Giacomo <edigiacomo@arpae.it> - 0.3-1
- Bug fix

* Wed May 15 2013 Daniele Branchini <dbranchini@carenza.metarpa> - 0.2-1
- Second release :)

* Mon Apr 22 2013 Emanuele Di Giacomo <edigiacomo@arpae.it>
- First release

%global releaseno 1
# Note: define _srcarchivename in Travis build only.
%{!?srcarchivename: %global srcarchivename %{name}-%{version}-%{releaseno}}

Name:           bufr2json
Version:        0.16
Release:        %{releaseno}%{?dist}
Summary:        BUFR to JSON converter
License:        GPLv2+
URL:            https://github.com/arpa-simc/%{name}
Source0:        https://github.com/arpa-simc/%{name}/archive/v%{version}-%{releaseno}.tar.gz#/%{srcarchivename}.tar.gz
BuildRequires:  python3
BuildRequires:  python3-dballe >= 8.0
Requires:       python3
Requires:       python3-dballe >= 8.0

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


%files
%doc
%{_bindir}/bufr2json

%changelog
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

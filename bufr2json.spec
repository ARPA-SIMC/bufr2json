Name:           bufr2json
Version:        0.7.1
Release:        1%{?dist}
Summary:        BUFR to JSON converter

License:        GPLv2+
URL:            http://www.arpa.emr.it/sim
Source0:        %{name}-%{version}.tar.gz

BuildRequires:  libdballe-devel >= 6.0
BuildRequires:  yajl-devel
BuildRequires:  help2man
Requires:       yajl
Requires:       dballe >= 6.0

%description
BUFR to JSON converter


%prep
%setup -q


%build
%configure
make %{?_smp_mflags}


%install
rm -rf $RPM_BUILD_ROOT
%make_install


%files
%doc
%{_bindir}/bufr2json
%doc %{_mandir}/man1/*



%changelog
* Wed Feb 19 2014 Emanuele Di Giacomo <edigiacomo@arpa.emr.it> - 0.7.1-1
- Optional geohash size
- Fixed segfault

* Thu Feb 06 2014 Emanuele Di Giacomo <edigiacomo@arpa.emr.it> - 0.6-1
- "lon" and "lat" properties

* Fri Jul 05 2013 Emanuele Di Giacomo <edigiacomo@arpa.emr.it> - 0.5-1
- Skip invalid data

* Fri Jul 05 2013 Emanuele Di Giacomo <edigiacomo@arpa.emr.it> - 0.4-1
- Data attributes and geohash support

* Tue Jul 02 2013 Emanuele Di Giacomo <edigiacomo@arpa.emr.it> - 0.3-1
- Bug fix

* Wed May 15 2013 Daniele Branchini <dbranchini@carenza.metarpa> - 0.2-1
- Second release :)

* Mon Apr 22 2013 Emanuele Di Giacomo <edigiacomo@arpa.emr.it>
- First release

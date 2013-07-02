Name:           bufr2json
Version:        0.3
Release:        1%{?dist}
Summary:        BUFR to JSON converter

License:        GPLv2+
URL:            http://www.arpa.emr.it/sim
Source0:        %{name}-%{version}.tar.gz

BuildRequires:  libdballe-devel >= 6.0
BuildRequires:  yajl-devel
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
* Tue Jul 02 2013 Emanuele Di Giacomo <edigiacomo@arpa.emr.it> - 0.3-1
- Bug fix

* Wed May 15 2013 Daniele Branchini <dbranchini@carenza.metarpa> - 0.2-1
- Second release :)

* Mon Apr 22 2013 Emanuele Di Giacomo <edigiacomo@arpa.emr.it>
- First release

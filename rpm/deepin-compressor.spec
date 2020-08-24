%define pkgrelease  1
%if 0%{?openeuler}
%define specrelease %{pkgrelease}
%else
## allow specrelease to have configurable %%{?dist} tag in other distribution
%define specrelease %{pkgrelease}%{?dist}
%endif



Name:           deepin-compressor
Version:        5.8.0.4
Release:        %{specrelease}
Summary:        Archive Manager is a fast and lightweight application for creating and extracting archives.
License:        GPLv3+
URL:            https://github.com/linuxdeepin/%{name}
Source0:        %{url}/archive/%{version}/%{name}-%{version}.tar.gz

BuildRequires: gcc-c++
BuildRequires: qt5-devel

BuildRequires: pkgconfig(dtkwidget)
BuildRequires: pkgconfig(deepin-gettext-tools)
# BuildRequires:  qt5-qtbase-devel
BuildRequires:  gsettings-qt-devel
BuildRequires:  udisks2-qt5-devel
# BuildRequires:  qt5-qtx11extras-devel
# BuildRequires:  qt5-qtmultimedia-devel
BuildRequires:  kf5-kcodecs
BuildRequires:  kf5-kcodecs-devel
BuildRequires:  libarchive-devel
BuildRequires:  libarchive
BuildRequires:  kf5-karchive-devel
BuildRequires:  libzip-devel
# BuildRequires:  qt5-linguist
BuildRequires:  libsecret-devel
BuildRequires:  poppler-cpp-devel
BuildRequires:  poppler-cpp
BuildRequires:  disomaster-devel
# BuildRequires:  qt5-qtsvg-devel
BuildRequires:  libminizip-devel
# BuildRequires:  zlib-devel

Requires: p7zip-plugins
Requires: pkgconfig(deepin-shortcut-viewer)



%description
%{summary}.

%prep
%autosetup

%build
# help find (and prefer) qt5 utilities, e.g. qmake, lrelease
export PATH=%{_qt5_bindir}:$PATH
mkdir build && pushd build
%qmake_qt5 ../ 
%make_build
popd

%install
%make_install -C build INSTALL_ROOT="%buildroot"

%files
%doc README.md
%license LICENSE
%{_bindir}/%{name}
%{_datadir}/icons/hicolor/scalable/apps/%{name}.svg
%{_datadir}/%{name}/translations/*.qm
# %{_datadir}/mime/packages/*.xml
%{_datadir}/applications/%{name}.desktop



%changelog

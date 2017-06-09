
# Maintainer: Joao Neves <joaoneves792@gmail.com>
reponame=cd00r
pkgname=${reponame%}-git
pkgver=r17.6790e2d
pkgrel=1
pkgdesc=""
arch=('x86_64')
url=""
license=('GPL')
groups=()
depends=('libpcap' 'iptables')
makedepends=('git' 'libpcap') 
provides=("${pkgname%}")
conflicts=("${pkgname%}")
replaces=()
backup=()
options=()
install=$pkgname.install
source=('git+https://warpenguin.no-ip.org/git/cd00r.git')
noextract=()
md5sums=('SKIP')


pkgver() {
	cd "$srcdir/${reponame%}"
# Git, no tags available
	printf "r%s.%s" "$(git rev-list --count HEAD)" "$(git rev-parse --short HEAD)"
}

prepare() {
	cd "$srcdir/${reponame%}"
}

build() {
	cd "$srcdir/${reponame%}"
	make
}

check() {
	cd "$srcdir/${reponame%}"
}

package() {
	cd "$srcdir/${reponame%}"
	mkdir -p $pkgdir/usr/lib/systemd/system
	mkdir -p $pkgdir/etc/tmpfiles.d
	mkdir -p $pkgdir/usr/bin
	mkdir -p $pkgdir/var/log/cd00rd

	cp ./cd00rd.service $pkgdir/usr/lib/systemd/system/
	cp ./cd00rd.conf $pkgdir/etc/tmpfiles.d/ 
	cp ./cd00rd $pkgdir/usr/bin/
	cp ./lock $pkgdir/usr/bin/

}

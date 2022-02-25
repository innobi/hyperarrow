set -e

PLATFORM="linux"
# TODO: we can likely take arguments for version, platform, etc...
# but right now we don't know the git hash being used by Tableau for the C++
# source
cpp_url="https://downloads.tableau.com/tssoftware/tableauhyperapi-cxx-${PLATFORM}-x86_64-release-hyperapi_release_26.0.0.13821.r1fbe38ce.zip"

mkdir /tmp/tableau
cd /tmp/tableau
wget -q ${cpp_url} -O tmp.zip
unzip -q tmp.zip -d /tmp/tableau
rm tmp.zip

mv tableauhyperapi-cxx-${PLATFORM}-x86_64-release-hyperapi_release_26.0.0.13821.r1fbe38ce tableauhyperapi

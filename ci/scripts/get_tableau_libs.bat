mkdir C:\tmp\tableau
curl -SL --output tmp.zip "https://downloads.tableau.com/tssoftware/tableauhyperapi-cxx-windows-x86_64-release-hyperapi_release_26.0.0.13821.r1fbe38ce.zip"
unzip -q tmp.zip -d C:\\tmp\\tableau
rm tmp.zip
pushd C:\\tmp\\tableau
mv tableauhyperapi-cxx-windows-x86_64-release-hyperapi_release_26.0.0.13821.r1fbe38ce tableauhyperapi
popd

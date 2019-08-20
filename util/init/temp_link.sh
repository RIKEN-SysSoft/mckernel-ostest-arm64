dd if=/dev/zero of=${temp} bs=1M count=10
ln -s ${temp} ${link}

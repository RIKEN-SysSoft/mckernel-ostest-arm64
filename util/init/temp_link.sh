printf 'hello'$'\0' > $temp
dd if=/dev/zero of=${temp} bs=1M count=10 obs=1 seek=6
ln -s ${temp} ${link}

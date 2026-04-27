cc ./md5.c -o md5sum
cp /usr/bin/md5sum /usr/bin/md5
mv ./md5sum /usr/bin/
chown root:root /usr/bin/md5sum
chmod 755 /usr/bin/md5sum

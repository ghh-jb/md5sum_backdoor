cc ./md5.c -o md5sum
mv ./md5sum /usr/local/bin/
chown root:root /usr/local/bin/md5sum
chmod 755 /usr/local/bin/md5sum
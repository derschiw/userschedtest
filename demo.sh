# Showcase default state

yes
yes > /dev/null &
yes > /dev/null &

# Normal policy 
su - root -c "userschedtest 0" &
su - user1 -c "userschedtest 0" &

top

# Reboot

# User policy
yes > /dev/null &
yes > /dev/null &

# Normal policy 
su - root -c "userschedtest 7" &
su - user1 -c "userschedtest 7" &

top

# Reboot
nano /proc/sys/kernel/usched_uid_weight

# Change content to this:
# 0000:2048,0100:256

top


# Reboot
userschedrun 101

# Reboot
userschedrun 102
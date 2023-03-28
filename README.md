# Rut
Rut is a simple one-file sudo-like utility. 

Originally from [CowNotGNU](https://github.com/CowNotGNU/CoreUtils)

## Configuring and installing
First begin by cloning this repo:
```sh
git clone https://github.com/TheMilkies/rut.git
cd rut
```

Now configure it as you'd like, edit the `hardcoded_users.conf` file:
```c
//{username, skip_password?}
{"johndoe", true}
{"kenth", false}
```

Now let's compile it, you must be logged in as root to do so.

1. Type `su` and enter your root password
2. Run `./install.sh`

## Credits
- Yogurt: Original developer.
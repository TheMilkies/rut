# Rut
Rut is a simple one-file sudo-like utility. It was originally from [CowNotGNU](https://github.com/CowNotGNU/CoreUtils) but we realized we should also have a hardcoded version. Rut is smol (at 89 lines of C code) which makes it a lot faster and more secure than sudo.

Since it's hardcoded, you'll need to recompile it every time you edit the configuration file.

## Advantages and disadvantages
Here are some advantages and disadvantages it has over other programs:
- ✅ A LOT smaller than sudo, 10% of the filesize.
- ✅ A LOT faster since it doesn't parse any file.
- ✅ Doesn't have any dependencies (like yacc (ahem doas)).
- ✅ Harder to exploit.
- ❌ Doesn't have a configuration file.
- ✅ Doesn't have a 120 line basic configuration file.
- ❌ Requires recompilation on config update.

## Configuring and installing
First begin by cloning this repo:
```sh
git clone https://github.com/TheMilkies/rut.git
cd rut
```

Now configure it as you'd like, edit the `users.conf` file:
```c
//{username, skip_password?}
{"johndoe", true}
{"kenth", false}
```

Now let's compile it, you must be logged in as root to do so.
1. Type `su` and enter your root password.
2. Run `./install.sh`

Now you have it in `/usr/bin`, Have fun!

## Credits
- Yogurt: Original developer.
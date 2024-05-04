# osu-safe

A proof of concept Windows
[FS minifilter driver](https://learn.microsoft.com/en-us/windows-hardware/drivers/ifs/about-file-system-filter-drivers)
that dynamically blocks osu! map backgrounds from loading on the fly.

Note that this requires
[TESTSIGNING](https://learn.microsoft.com/en-us/windows-hardware/drivers/install/the-testsigning-boot-configuration-option)
to be enabled in order to load the driver, which is incompatible with osu! score submission, and the anti-cheats of
many games for as long as TESTSIGNING is enabled. (ie, Vanguard, BE, EAC all refuse to launch)

You can technically use kdmapper or an EFI based mapper with Secure Boot disabled, but I heavily don't recommend doing
this. This is just a POC when playing offline (and in which case it's much easier to just patch the game directly)

## Details

Once the driver has been loaded, it will listen to all processes to find one named `osu!.exe`, verify it is osu!stable,
and then block any JPG and PNG files from having a file handle opened to them if they are located inside
the `Songs` folder.

The advantage of this is that you do not need to alter the map files, or do mass file renames every single time you
want to disable backgrounds, which is error-prone and very slow.
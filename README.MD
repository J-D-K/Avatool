## Avatool
Simple homebrew tool to overwrite Switch account avatars.

### Info
Avatool relies on shutting down the Switch's account service. This will result in a crash eventually. Once this is done:
* L and R change the target avatar
* Up and Down control menu
* A overwrites targetted avatars
	
Avatars are 256x256 jpg images. Place them in **sdmc:/avatars/** for Avatool to find them. When exporting your avatars, make sure they are not progressive as the switch cannot read them if they are. This results in a blank or white user icon.

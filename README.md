# CfgSwitcher
## v1.1.0-beta
### Description
This is the initial beta release of the CfgSwitcher application, the goal of which is to enable automatic switching of application (specifically game) configuration files based upon whether a laptop is on battery or wall power. 

Most laptops will reduce their performance and graphical power while unplugged, necessitating having to change game settings manually depending on a user's power situation.

With CfgSwitcher, a user can now grab the settings files of their games during both the plugged-in and unplugged states, and have them applied automatically when their power state changes.

### Use
Use of the application is extremely straight forward. The in-app menu allows the user to add a game to be switched, remove games from being switched, list the games currently setup for switching, and set the configs to be switched in for a given power state.

### Limitations
Currently, the beta application is a generic console app that cannot be minimized to the tray, and must be running for switching to take place. Additionally, there is minimal error checking of the app's settings file and file structure, meaning that any *manual* changes to the app's `configs` directory or `settings.xml` file may result in undefined and/or erroneous behavior. 

### What's Next
The next release of CfgSwitcher will see a Service-based solution in lieu of being console-based, with the addition of a basic GUI to control the service.

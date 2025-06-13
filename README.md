# minimize-dispatcher
A hyprland plugin to add minimize and unminimize dispatchers in addition to having the CSD minimize button work.

## Installation
```
hyprpm add https://github.com/khalid151/hyprland-minimize-plugin
hyprpm enable minimize-dispatcher
```

## Usage
Minimize the currently focused window.
```
hyprctl dispatch plugin:minimize:minimize
```

Minimize a specific window. Refer to https://wiki.hypr.land/Configuring/Dispatchers/#parameter-explanation
```
hyprctl dispatch plugin:minimize:minimize WINDOW
```

Select a specific window to unminimize.
```
hyprctl dispatch plugin:minimize:unminimize WINDOW
```

### Unminimize example
It's easily scriptable. Here's a little example using rofi as a menu to display a list of minimized windows to restore.

```
#!/bin/sh
MINIMIZED_CLIENTS=`hyprctl clients -j | jq -r '.[] | select(.workspace.name == "special:minimized")'`
RESTORE_ID=`echo $MINIMIZED_CLIENTS | jq --slurp -r '.[] | "\(.title)\u0000icon\u001f\(.class)"' | rofi -p "󱂬  Restore" -i -dmenu -format 'i'`
[ -z "$RESTORE_ID" ] && return
WINDOW_JSON=`echo $MINIMIZED_CLIENTS | jq --slurp -r ".[$RESTORE_ID]"`
ADDRESS=`echo $WINDOW_JSON | jq -r '.address'`
hyprctl dispatch plugin:minimize:unminimize address:$ADDRESS
```

#!/bin/sh
MINIMIZED_CLIENTS=`hyprctl clients -j | jq -r '.[] | select(.workspace.name == "special:minimized")'`
[ -z "$MINIMIZED_CLIENTS" ] && exit
RESTORE_ID=`echo $MINIMIZED_CLIENTS | jq --slurp -r '.[] | "\(.title)\u0000icon\u001f\(.class)"' | rofi -p "󱂬  Restore" -i -dmenu -format 'i'`
[ -z "$RESTORE_ID" ] && exit
WINDOW_JSON=`echo $MINIMIZED_CLIENTS | jq --slurp -r ".[$RESTORE_ID]"`
ADDRESS=`echo $WINDOW_JSON | jq -r '.address'`

hyprctl dispatch plugin:minimize:unminimize address:$ADDRESS

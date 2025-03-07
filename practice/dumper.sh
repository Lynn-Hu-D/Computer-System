#!/bin/bash

# Call arger with different arguments

echo "Calling arger with -u option (uppercase):"
./arger -u help is on the way
echo -e "\nExit code: $?"

echo "Calling arger with -l option (lowercase):"
./arger -l help is on the WAY
echo -e "\nExit code: $?"

echo "Calling arger with -cap option (capitalize first letter of each word):"
./arger -cap help is on the WAY
echo -e "\nExit code: $?"

echo "Calling arger with an invalid option:"
./arger -invalid help is on the WAY
echo -e "\nExit code: $?"

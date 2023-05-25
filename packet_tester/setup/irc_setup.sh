#!/bin/bash

mkdir -p /var/run/inspircd/

sleep 2;

exec inspircd --runasroot --nofork
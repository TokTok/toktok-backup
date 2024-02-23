#!/bin/sh

. /path/to/venv/bin/activate
exec github-backup "$@"

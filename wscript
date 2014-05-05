#! /usr/bin/env python
# encoding: utf-8

VERSION='0.9.0'
APPNAME='Vinary Search Tree'

top = 'src'
out = 'build'

def options(self):
  self.recurse('src')

def configure(self):
  self.recurse('src')

def build(self):
  self.recurse('src')

# vim: set et sta sw=2 ts=2:

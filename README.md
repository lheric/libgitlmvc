Introduction
============
[![Build Status](https://travis-ci.org/lheric/libgitlmvc.png?branch=master)](https://travis-ci.org/lheric/libgitlmvc)

libgitlmvc is a C++ MVC framework for Qt under BSD lisence.

How to build
============
Qt 5 with MSVC or GCC

Overview
========

View<====>Front Controller<====>Commands<====>Model

There are basically 4 parts in this framework:
* View: The user interface
* Front Controller(FC): Deal with all the request from View
* Commands: According to different request, FC call different commands. One command realize a specific function.
* Model: Store the state of the appliction, should only be changed by commands.

There are some differences with the traditional MVC. 
Firstly, there is no direct connection between views and model. It is a hierachical structure. Changes can be made without much side effects.

Secondly, we incorporate the front controller pattern to map one request to one command, and we use command parttern to separte different functions(or user case). These support a iterative and incremental development.

Get started
===========

TODO

You may refer to test/testcase.cpp

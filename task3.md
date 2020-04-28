# Task3

## What is a race condition ?

Happens when multiple processes run at the same time and use the same ressources. When a changes x and b changes x, then only the process which occurs last saves the data.

## Why do race condition occur ?
Because processes use the same ressource and change it at the same time. Then you have to handle the access.

## Is the output of a program with a race condition always incorrect ?
No, since the 2 processes have to access the element at the same time, the chance of it happening is very small.

## If you run your programm 100 times and always get the correct result, does that mean there is no race condition.
No, s.o.

## What is synchonization ?
When multiple processes coordinate ressources with eachother and wait for eachother

## Why are semaphores well suited for producer consumer pattern ?
Simple implementation and you can configure, after how many times they should wait for each other.

## Could Task2 also be implemented using a single semaphore ?
No, because a single semaphore couldn't communicate with another process and its semaphore.
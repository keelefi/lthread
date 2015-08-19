# Thread join design

Each thread must have it's own priority inheritance mutex. As long as the
thread is alive, the lock is held by the thread. When joining the thread,
simply try to grab the lock. The good thing with this design is that we will
detect a special deadlock where the joining thread holds lock A tries to join
with the other thread which however will wait for lock A before exiting.

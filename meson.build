project('Chat client server application', 'c')

client_src = ['client.c']
server_src = ['server.c', 'network.c']

List_lib = static_library('List', 'List/List.c')
List_inc = include_directories('List')
List = declare_dependency(link_with : List_lib, include_directories : List_inc)

ncurses = dependency('ncurses')
pthread = dependency('threads')

executable('server', server_src, dependencies : [ pthread, List ])
executable('client', client_src, dependencies : [ pthread, ncurses ])

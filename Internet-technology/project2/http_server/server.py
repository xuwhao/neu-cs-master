"""
StaticHttpServer request handler.
"""

import logging
import socket
from file_system.helper import get_file
from http_protocol.request import parse_http_request
from http_protocol.response import HttpResponse
from thread_pool.pool import ThreadPool
from config import RECV_BUFSIZ
from config import THREAD_POOL_SIZE
from config import SOCKET_BACKLOG_SIZE

Log = logging.getLogger('StaticHttpServer.server')


def handle_request(clientsock):

    data = clientsock.recv(RECV_BUFSIZ)

    Log.debug('Request received:\n%s', data)

    request = parse_http_request(data)

    request_uri = request.request_uri
    if request_uri.endswith('/'):
        request_uri = request_uri + 'index.html'

    file = get_file(request_uri)

    if file.exists and request.is_range_requested():
        response = HttpResponse(protocol=request.protocol, status_code=206,
                                range=request.get_range())
        response.file = file

    elif file.exists:
        response = HttpResponse(protocol=request.protocol, status_code=200)
        response.file = file

    else:

        response = HttpResponse(protocol=request.protocol, status_code=404)
        response.headers['Content-type'] = 'text/plain'
        response.content = 'This file does not exist!'

    Log.info('GET %s %s %s %s',
             request.request_uri, request.protocol, request.get_range(), response.status_code)

    response.write_to(clientsock)
    clientsock.close()


def run(host, port):
    address = (host, port)
    serversock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    serversock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    serversock.bind(address)
    serversock.listen(SOCKET_BACKLOG_SIZE)

    Log.info('StaticHttpServer started on %s:%s' % (host, port, ))

    pool = ThreadPool(THREAD_POOL_SIZE)

    while True:
        Log.debug('Waiting for connection...')

        clientsock, addr = serversock.accept()
        Log.debug('Connected from: %s', addr)

        pool.add_task(handle_request, clientsock)


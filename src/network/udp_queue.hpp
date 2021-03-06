/*
 * Copyright (C) 2013  Maxim Noah Khailo
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef FIRESTR_NETWORK_UDP_QUEUE_H
#define FIRESTR_NETWORK_UDP_QUEUE_H

#include "network/util.hpp"
#include "network/connection.hpp"
#include "network/message_queue.hpp"
#include "util/thread.hpp"

namespace fire
{
    namespace network
    {
        struct endpoint_message
        {
            endpoint ep;
            util::bytes data;
        };

        using endpoint_queue = util::queue<endpoint_message>;

        using udp_resolver_ptr = std::unique_ptr<boost::asio::ip::udp::resolver>;
        using udp_socket_ptr = std::unique_ptr<boost::asio::ip::udp::socket>;
        using sequence_type = uint64_t;
        using chunk_total_type = uint16_t;
        using chunk_id_type = uint16_t;

        struct udp_chunk
        {
            bool valid;
            std::string host;
            port_type port;
            sequence_type sequence;
            chunk_total_type total_chunks;
            chunk_id_type chunk;
            util::bytes data;
            enum msg_type { msg, ack} type;
        };

        using chunk_queue = util::queue<udp_chunk>;
        using udp_chunks = std::vector<udp_chunk>;

        struct working_udp_chunks
        {
            udp_chunks chunks;
            boost::dynamic_bitset<> set;
            size_t ticks = 0;
            size_t resent = 0;
        };

        using working_udp_endpoints = std::unordered_map<sequence_type, working_udp_chunks>;
        using working_udp_messages = std::unordered_map<std::string, working_udp_endpoints>;
        using resolve_map = std::unordered_map<std::string, std::string>;

        class udp_queue;
        class udp_connection
        {
            public:
                udp_connection(
                        endpoint_queue& in,
                        boost::asio::io_service& io, 
                        std::mutex& in_mutex);
            public:
                bool send(const endpoint_message& m, bool block = false);

            public:
                void bind(port_type port);
                void do_send(bool force);
                void handle_write(const boost::system::error_code& error);
                void handle_read(const boost::system::error_code& error, size_t transferred);
                void close();
                void start_read();
                void do_close();

            private:
                size_t chunkify(const std::string& host, port_type port, const fire::util::bytes& b);
                void send(udp_chunk& c);
                void resend();

            private:
                //reading
                util::bytes _in_buffer;
                boost::asio::ip::udp::endpoint _in_endpoint;
                working_udp_messages _in_working;
                working_udp_messages _out_working;
                std::mutex& _in_mutex;
                endpoint_queue& _in_queue;

                //writing
                chunk_queue _out_queue;
                util::bytes _out_buffer;

                //other
                boost::asio::io_service& _io;
                udp_socket_ptr _socket;
                sequence_type _sequence = 0;
                bool _writing;
                mutable std::mutex _mutex;
                boost::system::error_code _error;
            private:
                friend void udp_run_thread(udp_queue*);
                friend void resend_thread(udp_queue*);
        };

        using udp_connection_ptr = std::shared_ptr<udp_connection>;
        using udp_connections = std::vector<udp_connection_ptr>;

        class udp_queue
        {
            public:
                udp_queue(const asio_params& p);
                virtual ~udp_queue();

            public:
                virtual bool send(const endpoint_message& m);
                virtual bool receive(endpoint_message& b);

            private:
                void bind();
                const std::string& resolve(const endpoint&);

            private:
                asio_params _p;
                asio_service_ptr _io;
                util::thread_uptr _run_thread;
                util::thread_uptr _resend_thread;

                udp_connection_ptr _con;
                endpoint_queue _in_queue;
                udp_resolver_ptr _resolver;
                resolve_map _rmap;
                mutable std::mutex _mutex;
                bool _done;

            private:
                friend void udp_run_thread(udp_queue*);
                friend void resend_thread(udp_queue*);
        };

        using udp_queue_ptr = std::shared_ptr<udp_queue>;

        udp_queue_ptr create_udp_queue(const asio_params& c);
    }
}

#endif

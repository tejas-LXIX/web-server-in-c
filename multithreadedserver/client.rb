require 'socket'

s = TCPSocket.new 'localhost', 8989

s.write("/home/tejass/Desktop/web-server-in-c/multithreadedserver/tmp/testfiles/#{ARGV[0]}.txt\n")

s.each_line do |line|
    puts line
end

s.close
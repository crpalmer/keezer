require "socket"

module TemperatureHelper
  def get_temperatures
     connection = TCPSocket.open "0.0.0.0", 4567
     begin
        connection.puts "get_temperatures"
        connection.gets.split(" ").map { |v| v.to_f.round(1).to_s }
     ensure
        connection.close
     end
     rescue
	[ "???", "???", "???", "???", "???" ]
  end

  def keezer_temperature
     get_temperatures[3]
  end

  def tower_temperature
     get_temperatures[4]
  end

  def chamber_temperature
     get_temperatures[2]
  end

  def wort_temperature
     get_temperatures[1]
  end
end

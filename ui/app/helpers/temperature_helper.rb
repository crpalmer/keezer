require "socket"

module TemperatureHelper
  def server_command(cmd)
     connection = TCPSocket.open "0.0.0.0", 4567
     begin
        connection.puts cmd
        connection.gets
     ensure
        connection.close
     end
  end

  def get_temperatures
     server_command("get_temperatures").split(" ").map { |v| v.to_f }
     rescue
	[ -1, -1, -1, -1, -1 ]
  end

  def target_temperature
     get_temperatures[0]
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

  def fridge_is_on
     server_command("is_on").strip == "true"
  end
end

require "socket"

class Temperature
  include ActiveModel::Model

  def get_target_temperature
    get_temperature(0)
  end

  def get_wort_temperature
    get_temperature(1)
  end

  def get_chamber_temperature
    get_temperature(2)
  end

  def get_keezer_temperature
    get_temperature(3)
  end

  def get_tower_temperature
    get_temperature(4)
  end

  def is_fridge_on
    server_command("is_on") == "true"
  end

  def set_target_temperature(temperature)
    @temperatures = nil
    Rails.logger.debug "Temperature " + temperature
    server_command("set_temperature " + temperature) == "OK"
  end

  def finalize(c)
    c.connection.close
  end
  
  def get_last_server_command
    @last_server_command
  end

  def get_last_server_response
    @last_server_response
  end

private
  def get_temperature(i)
     get_temperatures
     @temperatures == nil ? -1 : @temperatures[i]
  end

  def get_temperatures
    if @temperatures == nil then
	@temperatures = server_command("get_temperatures").split(" ").map { |v| v.to_f }
    end
  end
   
  def server_command(cmd)
    if @connection == nil then
      @connection = TCPSocket.open "0.0.0.0", 4567
    end
    @last_server_command = cmd
    @connection.puts cmd
    @last_server_response = @connection.gets.strip
  end
end

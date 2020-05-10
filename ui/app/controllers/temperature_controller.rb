class TemperatureController < ApplicationController
  def initialize
    @temperature = Temperature.new
  end

  def edit
  end

  def set
     if @temperature.set_target_temperature(params["temperature"]["target_temperature"])
	redirect_to keezer_index_path
     else
	redirect_to temperature_edit_path(:error => "failed to set temperature: " + @temperature.get_last_server_response)
     end
  end

  def fridge_mode
    @temperature.set_fridge_mode
    redirect_to keezer_index_path
  end

  def fermenter_mode
    @temperature.set_fermenter_mode
    redirect_to keezer_index_path
  end

end


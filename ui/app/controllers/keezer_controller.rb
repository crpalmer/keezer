class KeezerController < ApplicationController
  def index
    @taps = Tap.all
    @temperature = Temperature.new
  end
end

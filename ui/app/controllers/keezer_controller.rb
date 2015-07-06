class KeezerController < ApplicationController
  def index
    @taps = Tap.all
  end
end

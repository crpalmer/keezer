class TapsController < ApplicationController
  def new
  end

  def index
    @taps = ordered_taps
    @beers = Beer.order("name").all
  end

  def create
    @tap = Tap.new
    taps = ordered_taps
    @tap.tap_number = taps.length+1
    @tap.save
    redirect_to taps_path
  end

  def update
    @tap = Tap.find(params[:id])
    @tap.update(tap_params)
    redirect_to taps_path
  end

  def destroy
    @tap = Tap.find(params[:id])
    @tap.destroy
    redirect_to taps_path
  end

  private
    def tap_params
      params.require(:tap).permit(:beer_id)
    end

    def ordered_taps
      Tap.order("tap_number").all
    end
end

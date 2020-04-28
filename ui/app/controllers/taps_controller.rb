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

  def edit
    @tap = Tap.find(params[:id]);
    @beers = Beer.order("name").all
  end

  def assign
    @tap = Tap.find(params[:id])
    @beer = Beer.find(params[:beer_id])
    @tap.beer_id = @beer.id
    if @tap.save then
	redirect_to root_path
    else
	render edit_path
    end
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

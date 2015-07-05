class BeersController < ApplicationController
  def new
  end

  def create
    @beer = Beer.new(beer_params)
    @beer.save
    redirect_to @beer
  end

  def show
    @beer = Beer.find(params[:id])
  end

  private
    def beer_params
      params.require(:beer).permit(:name, :brewer, :style, :ibu, :srm, :abv)
    end

end

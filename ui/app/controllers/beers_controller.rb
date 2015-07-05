class BeersController < ApplicationController
  def new
  end

  def index
    @beers = Beer.order("name").all
  end

  def create
    @beer = Beer.new(beer_params)
    @beer.save
    redirect_to @beer
  end

  def edit
    @beer = Beer.find(params[:id])
  end

  def update
    @beer = Beer.find(params[:id])
    if @beer.update(beer_params)
      redirect_to @beer
    else
      render 'edit'
    end
  end

  def show
    @beer = Beer.find(params[:id])
  end

  private
    def beer_params
      params.require(:beer).permit(:name, :brewer, :style, :ibu, :srm, :abv)
    end

end

Rails.application.routes.draw do
  get 'keezer/index'

  # The priority is based upon order of creation: first created -> highest priority.
  # See how all your routes lay out with "rake routes".

  # Beer management
  get 'beers/new/:tap_id', to: 'beers#new', as: 'new_beer'
  get 'beers/:id/edit', to: 'beers#edit', as: 'edit_beer'
  get 'beers/:id/edit/:tap_id', to: 'beers#edit', as: 'edit_beer_for_tap'
  delete 'beers/:id/delete/:tap_id', to: 'beers#delete', as: 'delete_beer_for_tap'
  post 'beers/:id/update', to: 'beers#update', as: 'update_beer'
  post 'beers/:id/update/:tap_id', to: 'beers#update', as: 'update_beer_for_tap'

  # Tap management
  get 'taps/new', to: 'taps#new', as: 'new_tap'
  get 'taps/delete-last', to: 'taps#delete_last', as: 'delete_last_tap'
  get 'taps/:id/edit', to: 'taps#edit', as: 'edit_tap'
  put 'taps/:id/assign/:beer_id', to: 'taps#assign', as: 'assign_tap'
  put 'taps/:id/unassign', to: 'taps#unassign', as: 'unassign_tap'

  # Temperature editing
  get 'temperature/edit' => 'temperature#edit'
  post 'temperature/set' => 'temperature#set'

  # You can have the root of your site routed with "root"
  root 'keezer#index'

  # Example of regular route:
  #   get 'products/:id' => 'catalog#view'

  # Example of named route that can be invoked with purchase_url(id: product.id)
  #   get 'products/:id/purchase' => 'catalog#purchase', as: :purchase

  # Example resource route (maps HTTP verbs to controller actions automatically):
  #   resources :products

  # Example resource route with options:
  #   resources :products do
  #     member do
  #       get 'short'
  #       post 'toggle'
  #     end
  #
  #     collection do
  #       get 'sold'
  #     end
  #   end

  # Example resource route with sub-resources:
  #   resources :products do
  #     resources :comments, :sales
  #     resource :seller
  #   end

  # Example resource route with more complex sub-resources:
  #   resources :products do
  #     resources :comments
  #     resources :sales do
  #       get 'recent', on: :collection
  #     end
  #   end

  # Example resource route with concerns:
  #   concern :toggleable do
  #     post 'toggle'
  #   end
  #   resources :posts, concerns: :toggleable
  #   resources :photos, concerns: :toggleable

  # Example resource route within a namespace:
  #   namespace :admin do
  #     # Directs /admin/products/* to Admin::ProductsController
  #     # (app/controllers/admin/products_controller.rb)
  #     resources :products
  #   end
end

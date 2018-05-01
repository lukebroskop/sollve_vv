(function (angular) {
  
  'use strict';
  
  var ompvv = angular.module('ompvvSollve', ['ngRoute','ngAnimate', 'ngSanitize', 'ui.bootstrap', 'ansiToHtml']);

  ompvv.controller('ompvvSollveController', 
  ['$scope', '$http', '$log','$uibModal', '$timeout', 'ansi2html',
    function($scope, $http, $log, $uibModal, $timeout, ansi2html) {

      // Info message regarding table
      $scope.displayMessage = true;
      $timeout(function(){
        $scope.displayMessage = false;
      }, 5000)

      // var to show and hide results loading
      $scope.loadingResults = true;

      // Var to show and hide error message if no results were found
      $scope.errorMessage = false;

      // Table variables and getting the json file from the server
      $scope.tableContent = [];
      $http.get("currentResults.json").then(function(data){
        $scope.tableContent = data.data;
        $scope.loadingResults = false;
        angular.forEach($scope.tableContent, function(value) {
          if ($scope.filters.compilerOptions.indexOf(value["Compiler name"]) == -1) {
            $scope.filters.compilerOptions.push(value["Compiler name"]);
          }
          if ($scope.filters.platformOptions.indexOf(value["Test system"]) == -1) {
            $scope.filters.platformOptions.push(value["Test system"]);
          }
        });
      },
      function(err) {
        $scope.loadingResults = false;
        $scope.errorMessage = true;
        $log.log("Error loading results json file " + err);
      });
      // To sort results by column
      $scope.order = {};
      $scope.order.reverseSort = false;
      $scope.order.orderByField = 'Test path';
      $scope.colsToDisplay = ['Test name', 'Test system', 'Compiler name', 'Compiler result', 'Runtime result'];

      // To filter results by column
      $scope.filters = {};
      $scope.filters.searchFilter = "";
      $scope.filters.compilerOptions = [""];
      $scope.filters.compilerFilter = [];
      $scope.filters.platformOptions = [""];
      $scope.filters.platformFilter = [];
      $scope.filters.compilerResultFilter = "Both";
      $scope.filters.runResultFilter = "Both";
      

      // Function for ordering table See here: 
      // https://stackoverflow.com/questions/23134773/angularjs-ngrepeat-orderby-when-property-key-has-spaces
      $scope.predicate = function(val) {
        return val[$scope.order.orderByField];
      }

      // function to verify if PASS is preset
      $scope.testPassed = function(result) {
        if (typeof result === 'string') {
          return result.indexOf("PASS") !== -1;
        } 
        return false;      }
      $scope.testFailed = function(result) {
        if (typeof result === 'string') {
          return result.indexOf("FAIL") !== -1;
        } 
        return false;
      }


      // More information results modal
      $scope.openResultModal = function (value) {
        var modalInstance = $uibModal.open({
          animation: true,
          component: 'modalResultado',
          size: 'lg',
          resolve: {
            resultEntry: function() {
              return value;
            },
            ansi2html: function() {
              return ansi2html;
            }

          }
        });

        modalInstance.result.then(function () {
          $log.info('Modal closed');
        }, function () {
          $log.info('Modal dismissed');
        });
      }
  }]);

  var modalResultadoCtrl = 

  ompvv.component('modalResultado', {
    templateUrl: 'resultadoModalTemplate.html',
    controllerAs: '$ctrl',
    bindings: {
      resolve: '<',
      close: '&'
    },
    controller: function($sce) {
      var $ctrl = this;
      $ctrl.compilerOutputActive = false;
      $ctrl.runtimeOutputActive = false;
      $ctrl.$onInit = function () {
        $ctrl.resultEntry = $ctrl.resolve.resultEntry;
        $ctrl.ansi2html = $ctrl.resolve.ansi2html
      }
      $ctrl.ok = function () {
        $uibModalInstance.close();
      };
    
      $ctrl.cancel = function () {
        $uibModalInstance.dismiss('cancel');
      };
  
      // function to verify if PASS is preset
      $ctrl.testPassed = function(result) {
        if (typeof result === 'string') {
          return result.indexOf("PASS") !== -1;
        } 
        return false;      }
      $ctrl.testFailed = function(result) {
        if (typeof result === 'string') {
          return result.indexOf("FAIL") !== -1;
        } 
        return false;
      }
      $ctrl.color2html = function(outputText) {
        return $sce.trustAsHtml($ctrl.ansi2html.toHtml(outputText).replace(/\n/g, '<br/>'));
      }
  
    }
  });
  
  ompvv.filter('applyResultsFilter', function() {
      return function (items, filters) {
        var filtered = [];
        if (filters.searchFilter == "" && filters.compilerFilter == "" && filters.platformFilter == "" && filters.compilerResultFilter == "Both" && filters.runResultFilter == "Both") {
          return items;
        }
        angular.forEach(items, function(item) {
          var removeItem = false;
          // filter by test name 
          if (filters.searchFilter != "" && !item['Test path'].includes(filters.searchFilter))
            removeItem = true;
          // Filter by compiler name 
          var containsCompiler = false;
          angular.forEach(filters.compilerFilter, function(compiler) {
            if (compiler != "" && item['Compiler name'] == compiler)
              containsCompiler = true;
          });
          if (filters.compilerFilter != "" && !containsCompiler)
            removeItem = true;
          // Filter by platform name
          var containsPlatform = false;
          angular.forEach(filters.platformFilter, function(platform) {
            if (platform != "" && item['Test system'] == platform)
              containsPlatform = true;
          });
          if (filters.platformFilter != "" && !containsPlatform)
            removeItem = true;
          // Filter by compiler result
          if (filters.compilerResultFilter != "Both" && !item['Compiler result'].includes(filters.compilerResultFilter))
            removeItem = true;
          // Filter by Runtime result
          if (filters.runResultFilter != "Both" && !item['Runtime result'].includes(filters.runResultFilter))
            removeItem = true;
          if (!removeItem) 
            filtered.push(item);
        });
        return filtered;
      };
  });
  // Just in case we need it
//   ompvv.config(function($routeProvider){
//     $routeProvider.caseInsensitiveMatch = true;    
//     $routeProvider
//       .when('/', {
//         templateUrl: 'views/home.html',
//         controller:  'indexController'
//       })
//       .otherwise({
//         redirectTo:  '/'
//       });
//   })

})(window.angular);
